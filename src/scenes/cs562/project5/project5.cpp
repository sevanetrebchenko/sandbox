
#include "scenes/cs562/project5/project5.h"
#include "scenes/cs562/project3/skydome.h"
#include "scenes/cs562/project3/shadow_caster.h"
#include "common/api/window.h"
#include "common/geometry/object_loader.h"
#include "common/utility/log.h"
#include "common/geometry/transform.h"
#include "common/ecs/ecs.h"
#include "common/application/time.h"
#include "common/api/buffer/vao_manager.h"

#include "scenes/cs562/project5/refractive.h"

namespace Sandbox {

    static const int PHONG = 0;
    static const int GGX = 1;
    static const int BECKMAN = 2;

    SceneCS562Project5::SceneCS562Project5() : fbo_(2560, 1440),
                                               shadowMap_(2048, 2048),
                                               camera_(Window::Instance().GetWidth(), Window::Instance().GetHeight()),
                                               blurKernelRadius_(25),
                                               brdfModel_(PHONG),
                                               environmentMap_("environment"),
                                               irradianceMap_("irradiance"),
                                               exposure_(3.0f),
                                               contrast_(2.0f),
                                               refractionFBO_(2048, 2048),
                                               causticFBO_(1024, 576),
                                               vertexGrid_(VAOManager::Instance().GetVAO("assets/models/sphere.obj"))
                                               {
    }

    SceneCS562Project5::~SceneCS562Project5() {
    }

    void SceneCS562Project5::OnInit() {
        IScene::OnInit();

        InitializeShaders();
        InitializeMaterials();
        InitializeTextures();

        ConfigureLights();
        ConfigureModels();
        ConstructFBO();
        ConstructShadowMap();
        InitializeBlurKernel();
        GenerateRandomPoints();

        ConstructRefractionFBO();
        ConstructCausticFBO();
        InitializeVertexGrid();

        camera_.SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    }

    void SceneCS562Project5::OnUpdate() {
        IScene::OnUpdate();
        camera_.Update();

        // Rotate center model.
//        ComponentWrapper<Transform> transform = ECS::Instance().GetComponent<Transform>("Bunny");
//        transform->SetRotation(transform->GetRotation() + glm::vec3(0.0f, 10.0f, 0.0f) * Time::Instance().dt);
    }

    void SceneCS562Project5::OnPreRender() {
        IScene::OnPreRender();
    }

    void SceneCS562Project5::OnRender() {
        IScene::OnRender();

        Backend::Core::EnableFlag(GL_DEPTH_TEST);

        // Render shadow map.
        glm::vec4 viewport = Backend::Core::GetViewport();




        Backend::Core::SetViewport(0, 0, shadowMap_.GetWidth(), shadowMap_.GetHeight());
        shadowMap_.BindForReadWrite();

        GenerateShadowMap();
        BlurShadowMap();

        // Restore viewport.
        shadowMap_.Unbind();
        Backend::Core::SetViewport(viewport.x, viewport.y, viewport.z, viewport.w);




        Backend::Core::SetViewport(0, 0, causticFBO_.GetWidth(), causticFBO_.GetHeight());
        causticFBO_.BindForReadWrite();

        CausticMapping();

        // Restore viewport.
        causticFBO_.Unbind();
        Backend::Core::SetViewport(viewport.x, viewport.y, viewport.z, viewport.w);




        // Render refractive cubemap.
        Backend::Core::SetViewport(0, 0, refractionFBO_.GetWidth(), refractionFBO_.GetHeight());
        refractionFBO_.BindForReadWrite();

        GenerateCubemap();

        refractionFBO_.Unbind();
        Backend::Core::SetViewport(viewport.x, viewport.y, viewport.z, viewport.w);




        Backend::Core::SetViewport(0, 0, fbo_.GetWidth(), fbo_.GetHeight());
        fbo_.BindForReadWrite();

        // 1. G-buffer pass.
        GeometryPass();

        Backend::Core::DisableFlag(GL_DEPTH_TEST);

        // 2. Global lighting pass.
        GlobalLightingPass();

        glDepthMask(GL_FALSE); // Don't write to depth buffer.

        // 3. Local lighting pass.
        Backend::Core::EnableFlag(GL_BLEND); // Enable blending.
        glBlendFunc(GL_ONE, GL_ONE);         // Additive blending TODO: abstract out.

        Backend::Core::EnableFlag(GL_CULL_FACE);
        Backend::Core::CullFace(GL_FRONT);

        // LocalLightingPass();

        Backend::Core::DisableFlag(GL_CULL_FACE);
        Backend::Core::DisableFlag(GL_BLEND);

        Backend::Core::DisableFlag(GL_BLEND); // Disable blending.

        glDepthMask(GL_TRUE);
        Backend::Core::EnableFlag(GL_DEPTH_TEST);

        RenderRefractiveObject();
        RenderSkydome();

        // Restore viewport.
        fbo_.Unbind();
        Backend::Core::SetViewport(viewport.x, viewport.y, viewport.z, viewport.w);
    }

    void SceneCS562Project5::OnPostRender() {
        IScene::OnPostRender();
    }

    void SceneCS562Project5::OnImGui() {
        // Enable window docking.
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        IScene::OnImGui();

        // Framework overview.
        static bool showOverview = true;
        if (ImGui::Begin("Overview"), &showOverview) {
            ImGui::Text("Render time:");
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::Separator();

            if (ImGui::Button("Take Screenshot")) {
                refractionFBO_.SaveRenderTargetsToDirectory("data/scenes/cs562/project5/");
                causticFBO_.SaveRenderTargetsToDirectory("data/scenes/cs562/project5/");
            }
        }
        ImGui::End();

        // Ensure proper scene image scaling.
        Window& window = Window::Instance();

        // Draw the final output from the hybrid rendering pipeline.
        static bool showFinalOutput = true;
        float aspectRatio = camera_.GetAspectRatio();

        if (ImGui::Begin("Framebuffer"), &showFinalOutput) {
            float maxWidth = ImGui::GetWindowContentRegionWidth();
            ImVec2 imageSize = ImVec2(maxWidth, maxWidth / aspectRatio);

            ImGui::SetCursorPosY(ImGui::GetItemRectSize().y + (ImGui::GetWindowSize().y - ImGui::GetItemRectSize().y - imageSize.y) * 0.5f);
            ImGui::Image(reinterpret_cast<ImTextureID>(fbo_.GetNamedRenderTarget("output")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();

        // Refractive framebuffer color attachments.
        static bool showEnvironmentMap = false;
        if (ImGui::Begin("Environment Mapping Textures"), &showEnvironmentMap) {
            float maxWidth = ImGui::GetWindowContentRegionWidth();
            ImVec2 imageSize = ImVec2(maxWidth, maxWidth / (static_cast<float>(refractionFBO_.GetWidth()) / static_cast<float>(refractionFBO_.GetHeight())));

            ImGui::Text("Positive X:");
            ImGui::Image(reinterpret_cast<ImTextureID>(refractionFBO_.GetNamedRenderTarget("posX")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Negative X:");
            ImGui::Image(reinterpret_cast<ImTextureID>(refractionFBO_.GetNamedRenderTarget("negX")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Positive Y:");
            ImGui::Image(reinterpret_cast<ImTextureID>(refractionFBO_.GetNamedRenderTarget("posY")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Negative Y:");
            ImGui::Image(reinterpret_cast<ImTextureID>(refractionFBO_.GetNamedRenderTarget("negY")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Positive Z:");
            ImGui::Image(reinterpret_cast<ImTextureID>(refractionFBO_.GetNamedRenderTarget("posZ")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Negative Z:");
            ImGui::Image(reinterpret_cast<ImTextureID>(refractionFBO_.GetNamedRenderTarget("negZ")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();
        }
        ImGui::End();

        static bool showShadowMap = false;
        if (ImGui::Begin("Shadow Map Textures"), &showShadowMap) {
            float maxWidth = ImGui::GetWindowContentRegionWidth();
            ImVec2 imageSize = ImVec2(maxWidth, maxWidth / (static_cast<float>(shadowMap_.GetWidth()) / static_cast<float>(shadowMap_.GetHeight())));

            {
                Texture* shadowMap = shadowMap_.GetNamedRenderTarget("depth output");
                ImGui::Text("Shadow Map:");
                ImGui::Image(reinterpret_cast<ImTextureID>(shadowMap->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
                ImGui::Separator();
            }

            {
                ImGui::Text("Blur Kernel Radius: ");
                if (ImGui::SliderInt("##kernelRadius", &blurKernelRadius_, 0, 50)) {
                    InitializeBlurKernel();
                }

                Texture* shadowMap = shadowMap_.GetNamedRenderTarget("blur output");
                ImGui::Text("Blurred Shadow Map:");
                ImGui::Image(reinterpret_cast<ImTextureID>(shadowMap->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            }

            ImGui::End();
        }

        // Draw individual deferred rendering textures.
        static bool showGBufferTextures = false;
        if (ImGui::Begin("GBuffer Textures"), &showGBufferTextures) {
            float maxWidth = ImGui::GetWindowContentRegionWidth();
            ImVec2 imageSize = ImVec2(maxWidth, maxWidth / aspectRatio);

            ImGui::Text("Position (world space):");
            ImGui::Image(reinterpret_cast<ImTextureID>(fbo_.GetNamedRenderTarget("position")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Normals (world space):");
            ImGui::Image(reinterpret_cast<ImTextureID>(fbo_.GetNamedRenderTarget("normal")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Ambient component:");
            ImGui::Image(reinterpret_cast<ImTextureID>(fbo_.GetNamedRenderTarget("ambient")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Diffuse component:");
            ImGui::Image(reinterpret_cast<ImTextureID>(fbo_.GetNamedRenderTarget("diffuse")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Specular component:");
            ImGui::Image(reinterpret_cast<ImTextureID>(fbo_.GetNamedRenderTarget("specular")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Scene depth:");
            ImGui::Image(reinterpret_cast<ImTextureID>(fbo_.GetNamedRenderTarget("depth")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();
        }
        ImGui::End();

        static bool showCausticMap = false;
        if (ImGui::Begin("Caustic Map Textures"), &showCausticMap) {
            float maxWidth = ImGui::GetWindowContentRegionWidth();
            ImVec2 imageSize = ImVec2(maxWidth, maxWidth / (static_cast<float>(causticFBO_.GetWidth()) / static_cast<float>(causticFBO_.GetHeight())));

            ImGui::Text("Receiver Positions:");
            ImGui::Image(reinterpret_cast<ImTextureID>(causticFBO_.GetNamedRenderTarget("receiver positions")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Refractive Positions:");
            ImGui::Image(reinterpret_cast<ImTextureID>(causticFBO_.GetNamedRenderTarget("refractive positions")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Refractive Normals:");
            ImGui::Image(reinterpret_cast<ImTextureID>(causticFBO_.GetNamedRenderTarget("refractive normals")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Caustic Mapping Output:");
            ImGui::Image(reinterpret_cast<ImTextureID>(causticFBO_.GetNamedRenderTarget("output")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::End();
        }

        static bool showSceneConfiguration = false;
        if (ImGui::Begin("Scene Configuration"), &showSceneConfiguration) {
            ImGui::Text("Exposure: ");
            ImGui::SliderFloat("##exposure", &exposure_, 0.1f, 100.0f);

            ImGui::Text("Contrast: ");
            ImGui::SliderFloat("##contrast", &contrast_, 0.0f, 5.0f);
        }
        ImGui::End();

        ImGuiLog::Instance().OnImGui();
    }

    void SceneCS562Project5::OnShutdown() {
        IScene::OnShutdown();
    }

    void SceneCS562Project5::OnWindowResize(int width, int height) {
        IScene::OnWindowResize(width, height);

        // Rebuild swapchain.
        fbo_.Reallocate(width, height);
        if (!fbo_.CheckStatus()) {
            throw std::runtime_error("Custom FBO is not complete.");
        }

        // Update camera aspect ratio.
        camera_.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));

        Backend::Core::SetViewport(0, 0, width, height);
    }

    void SceneCS562Project5::InitializeShaders() {
        ShaderLibrary& shaderLibrary = ShaderLibrary::Instance();

        shaderLibrary.CreateShader("Geometry Pass", { "assets/shaders/geometry_buffer.vert", "assets/shaders/geometry_buffer.frag" });
        shaderLibrary.CreateShader("Global Lighting BRDF Pass", { "assets/shaders/global_brdf.vert", "assets/shaders/global_brdf.frag" });
        shaderLibrary.CreateShader("Local Lighting BRDF Pass", { "assets/shaders/local_brdf.vert", "assets/shaders/local_brdf.frag" });
        shaderLibrary.CreateShader("FSQ", { "assets/shaders/fsq.vert", "assets/shaders/fsq.frag" });

        shaderLibrary.CreateShader("Shadow Pass", { "assets/shaders/shadow.vert", "assets/shaders/shadow.frag" });
        shaderLibrary.CreateShader("Depth Pass", { "assets/shaders/fsq.vert", "assets/shaders/depth.frag" });
        shaderLibrary.CreateShader("Depth Out", { "assets/shaders/fsq.vert", "assets/shaders/depth_out.frag" });
        shaderLibrary.CreateShader("Blur Horizontal", { "assets/shaders/blur_horizontal.comp" });
        shaderLibrary.CreateShader("Blur Vertical", { "assets/shaders/blur_vertical.comp" });

        shaderLibrary.CreateShader("Skydome", { "assets/shaders/skydome.vert", "assets/shaders/skydome.frag" });

        shaderLibrary.CreateShader("Global Lighting BRDF Forward Pass", { "assets/shaders/global_brdf_forward.vert", "assets/shaders/global_brdf_forward.frag" });
        shaderLibrary.CreateShader("Environment Mapping", { "assets/shaders/environment_mapping.vert", "assets/shaders/environment_mapping.frag" });

        shaderLibrary.CreateShader("Caustic Receiver", { "assets/shaders/caustic_pass.vert", "assets/shaders/caustic_receiver.frag" });
        shaderLibrary.CreateShader("Caustic Refractive", { "assets/shaders/caustic_pass.vert", "assets/shaders/caustic_refractive.frag" });
        shaderLibrary.CreateShader("Caustic Mapping", { "assets/shaders/caustic_mapping.vert", "assets/shaders/caustic_mapping.frag" });
    }

    void SceneCS562Project5::InitializeMaterials() {
        // Phong shading material.
        Material* phongMaterial = new Material("Phong", {
                { "ambientCoefficient", glm::vec3(0.5f) },
                { "diffuseCoefficient", glm::vec3(0.5f) },
                { "specularCoefficient", glm::vec3(1.0f) },
                { "specularExponent", 2.0f }
        });
        phongMaterial->GetUniform("ambientCoefficient")->UseColorPicker(true);
        phongMaterial->GetUniform("diffuseCoefficient")->UseColorPicker(true);
        phongMaterial->GetUniform("specularCoefficient")->UseColorPicker(true);
        phongMaterial->GetUniform("specularExponent")->SetSliderRange(0.0f, 100.0f);
        materialLibrary_.AddMaterial(phongMaterial);
    }

    void SceneCS562Project5::ConfigureModels() {
        ECS& ecs = ECS::Instance();

        // Bunny.
        {
            int bunny = ecs.CreateEntity("Bunny");
            ecs.AddComponent<Mesh>(bunny, OBJLoader::Instance().LoadFromFile(OBJLoader::Request("assets/models/bunny_high_poly.obj"))).Configure([](Mesh& mesh) {
                mesh.Complete();
                mesh.SetActive(false);
            });
//            ecs.AddComponent<Mesh>(bunny, OBJLoader::Instance().LoadSphere()).Configure([](Mesh& mesh) {
//                mesh.Complete();
//                mesh.SetActive(true);
//            });
            ecs.AddComponent<MaterialCollection>(bunny).Configure([this](MaterialCollection& materialCollection) {
                Material* phong = materialLibrary_.GetMaterialInstance("Phong");
                phong->GetUniform("ambientCoefficient")->SetData(glm::vec3(0.05f));
                phong->GetUniform("diffuseCoefficient")->SetData(glm::vec3(1.0f));
                phong->GetUniform("specularCoefficient")->SetData(glm::vec3(1.0f));
                phong->GetUniform("specularExponent")->SetData(50.0f);

                materialCollection.SetMaterial(phong);
            });
            ecs.AddComponent<ShadowCaster>(bunny);
            ecs.AddComponent<Refractive>(bunny).Configure([](Refractive& refractive) {
                refractive.ior = 1.33f; // Water.
            });
        }

        // Floor.
        {
            int floor = ecs.CreateEntity("Floor");
            ecs.AddComponent<Mesh>(floor, OBJLoader::Instance().LoadFromFile(OBJLoader::Request("assets/models/quad.obj"))).Configure([](Mesh& mesh) {
                mesh.Complete();
                mesh.SetActive(true);
            });
            ecs.AddComponent<MaterialCollection>(floor).Configure([this](MaterialCollection& materialCollection) {
                Material* phong = materialLibrary_.GetMaterialInstance("Phong");
                phong->GetUniform("ambientCoefficient")->SetData(glm::vec3(0.2f));
                phong->GetUniform("diffuseCoefficient")->SetData(glm::vec3(0.1f));
                phong->GetUniform("specularCoefficient")->SetData(glm::vec3(0.8f));
                phong->GetUniform("specularExponent")->SetData(1400.0f);

                materialCollection.SetMaterial(phong);
            });
            ecs.GetComponent<Transform>(floor).Configure([](Transform& transform) {
                transform.SetPosition(glm::vec3(0.0f, -1.3f, 0.0f));
                transform.SetScale(glm::vec3(5.0f));
                transform.SetRotation(glm::vec3(270.0f, 0.0f, 0.0f));
            });
            ecs.AddComponent<ShadowCaster>(floor);
        }

//        // Sphere.
//        {
//            int sphere = ecs.CreateEntity("Sphere");
//            ecs.AddComponent<Mesh>(sphere, OBJLoader::Instance().LoadSphere()).Configure([](Mesh& mesh) {
//                mesh.Complete();
//                mesh.SetActive(true);
//            });
//            ecs.AddComponent<MaterialCollection>(sphere).Configure([this](MaterialCollection& materialCollection) {
//                Material* phong = materialLibrary_.GetMaterialInstance("Phong");
//                phong->GetUniform("ambientCoefficient")->SetData(glm::vec3(0.2f));
//                phong->GetUniform("diffuseCoefficient")->SetData(glm::vec3(0.1f));
//                phong->GetUniform("specularCoefficient")->SetData(glm::vec3(0.8f));
//                phong->GetUniform("specularExponent")->SetData(1400.0f);
//
//                materialCollection.SetMaterial(phong);
//            });
//            ecs.GetComponent<Transform>(sphere).Configure([](Transform& transform) {
//                transform.SetPosition(glm::vec3(-15.0f, 0.0f, 0.0f));
//                transform.SetScale(glm::vec3(2.0f));
//                transform.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
//            });
//            ecs.AddComponent<ShadowCaster>(sphere);
//        }

        // Compute scene bounds.
        ecs.IterateOver<Transform, Mesh>([this](Transform& transform, Mesh& mesh) {
            // Compute object bounds.
            std::vector<glm::vec3> vertices = mesh.GetVertices();
            glm::mat4 matrix = transform.GetMatrix();

            for (const glm::vec3& vertex : vertices) {
                bounds_.Extend(matrix * glm::vec4(vertex, 1.0f));
            }
        });

        // Skydome.
        {
            int skydome = ecs.CreateEntity("Skydome");
//            ecs.AddComponent<Mesh>(skydome, OBJLoader::Instance().LoadFromFile(OBJLoader::Request("assets/models/sphere.obj"))).Configure([](Mesh& mesh) {
//                mesh.Complete();
//            });
            ecs.AddComponent<Mesh>(skydome, OBJLoader::Instance().LoadSphere()).Configure([](Mesh& mesh) {
                mesh.Complete();
                mesh.SetActive(true);
            });
            ecs.AddComponent<Skydome>(skydome);
            ecs.GetComponent<Transform>(skydome).Configure([this](Transform& transform) {
                float max = std::numeric_limits<float>::lowest();
                for (int axis = 0; axis < 3; ++axis) {
                    if (bounds_.GetDiagonal()[axis] > max) {
                        max = bounds_.GetDiagonal()[axis];
                    }
                }

                transform.SetScale(glm::vec3(max * 5.0f));
            });
        }
    }

    void SceneCS562Project5::ConfigureLights() {
        directionalLight_.brightness_ = 1.0f;

        ECS& ecs = ECS::Instance();
        Mesh mesh = OBJLoader::Instance().LoadFromFile(OBJLoader::Request("assets/models/sphere.obj"));

        // Orange light.
        {
            int ID = ecs.CreateEntity("light 1");
            ecs.AddComponent<Mesh>(ID, mesh).Configure([](Mesh& mesh) {
                mesh.Complete();
            });

            ecs.GetComponent<Transform>(ID).Configure([](Transform& transform) {
                transform.SetPosition(glm::vec3(2.0f, 0.0f, 0.0f));
                transform.SetScale(glm::vec3(4.0f));
            });
            ecs.AddComponent<LocalLight>(ID, glm::vec3(1.0f, 0.6f, 0.25f), 1.0f);
        }

        // Purple light.
        {
            int ID = ecs.CreateEntity("light 2");
            ecs.AddComponent<Mesh>(ID, mesh).Configure([](Mesh& mesh) {
                mesh.Complete();
            });

            ecs.GetComponent<Transform>(ID).Configure([](Transform& transform) {
                transform.SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
                transform.SetScale(glm::vec3(4.0f));
            });
            ecs.AddComponent<LocalLight>(ID, glm::vec3(0.5f, 0.3f, 0.8f), 1.0f);
        }

        // Light volumes cast no shadows.
        // ecs.AddComponent<ShadowCaster>(ID);
    }

    void SceneCS562Project5::ConstructFBO() {
        int contentWidth = 2560;
        int contentHeight = 1440;

        fbo_.BindForReadWrite();

        // Position (world space).
        Texture* positionTexture = new Texture("position");
        positionTexture->Bind();
        positionTexture->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        positionTexture->Unbind();
        fbo_.AttachRenderTarget(positionTexture);

        // Normals (world space).
        Texture* normalTexture = new Texture("normal");
        normalTexture->Bind();
        normalTexture->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        normalTexture->Unbind();
        fbo_.AttachRenderTarget(normalTexture);

        // Ambient color.
        Texture* ambientTexture = new Texture("ambient");
        ambientTexture->Bind();
        ambientTexture->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        ambientTexture->Unbind();
        fbo_.AttachRenderTarget(ambientTexture);

        // Diffuse color.
        Texture* diffuseTexture = new Texture("diffuse");
        diffuseTexture->Bind();
        diffuseTexture->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        diffuseTexture->Unbind();
        fbo_.AttachRenderTarget(diffuseTexture);

        // Specular color.
        Texture* specularTexture = new Texture("specular");
        specularTexture->Bind();
        specularTexture->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        specularTexture->Unbind();
        fbo_.AttachRenderTarget(specularTexture);

        // Depth texture (for visualizing depth information).
        Texture* depthTexture = new Texture("depth");
        depthTexture->Bind();
        depthTexture->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        depthTexture->Unbind();
        fbo_.AttachRenderTarget(depthTexture);

        // Output texture.
        Texture* outputTexture = new Texture("output");
        outputTexture->Bind();
        outputTexture->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        outputTexture->Unbind();
        fbo_.AttachRenderTarget(outputTexture);

        // Depth buffer.
        Texture* depthBuffer = new Texture("depth buffer");
        depthBuffer->Bind();
        depthBuffer->ReserveData(Texture::AttachmentType::DEPTH, contentWidth, contentHeight);
        depthBuffer->Unbind();
        fbo_.AttachRenderTarget(depthBuffer);

        // Depth buffer (RBO).
//        RenderBufferObject* depthBuffer = new RenderBufferObject();
//        depthBuffer->Bind();
//        depthBuffer->ReserveData(contentWidth, contentHeight);
//        fbo_.AttachDepthBuffer(depthBuffer);

        if (!fbo_.CheckStatus()) {
            throw std::runtime_error("Custom FBO is not complete.");
        }

        fbo_.Unbind();
    }

    void SceneCS562Project5::ConstructShadowMap() {
        int contentWidth = 2048;
        int contentHeight = 2048;

        shadowMap_.BindForReadWrite();

        // Texture for visually debugging the shadow map pass.
        Texture* outputTexture = new Texture("depth output");
        outputTexture->Bind();
        outputTexture->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        outputTexture->Unbind();
        shadowMap_.AttachRenderTarget(outputTexture);

        Texture* depthTexture = new Texture("depth");
        depthTexture->Bind();
        depthTexture->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        depthTexture->Unbind();
        shadowMap_.AttachRenderTarget(depthTexture);

        // Texture for visually debugging the shadow map blurring pass.
        Texture* blurTexture = new Texture("blur output");
        blurTexture->Bind();
        blurTexture->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        blurTexture->Unbind();
        shadowMap_.AttachRenderTarget(blurTexture);

        // Textures for compute shader blurring.
        Texture* blurHorizontalTexture = new Texture("blur horizontal");
        blurHorizontalTexture->Bind();
        blurHorizontalTexture->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        blurHorizontalTexture->Unbind();
        shadowMap_.AttachRenderTarget(blurHorizontalTexture);

        Texture* blurVerticalTexture = new Texture("blur vertical");
        blurVerticalTexture->Bind();
        blurVerticalTexture->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        blurVerticalTexture->Unbind();
        shadowMap_.AttachRenderTarget(blurVerticalTexture);

        Texture* depthBuffer = new Texture("depth buffer");
        depthBuffer->Bind();
        depthBuffer->ReserveData(Texture::AttachmentType::DEPTH, contentWidth, contentHeight);
        depthBuffer->Unbind();
        shadowMap_.AttachRenderTarget(depthBuffer);

//        // Depth buffer (RBO).
//        RenderBufferObject* depthBuffer = new RenderBufferObject();
//        depthBuffer->Bind();
//        depthBuffer->ReserveData(contentWidth, contentHeight);
//        shadowMap_.AttachDepthBuffer(depthBuffer);

        if (!shadowMap_.CheckStatus()) {
            throw std::runtime_error("Custom Shadow FBO is not complete.");
        }

        shadowMap_.Unbind();
    }

    void SceneCS562Project5::GeometryPass() {
        fbo_.DrawBuffers(0, 5);
        Backend::Core::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear everything for a new scene.

        Shader* geometryShader = ShaderLibrary::Instance().GetShader("Geometry Pass");
        geometryShader->Bind();

        // Set camera uniforms.
        geometryShader->SetUniform("cameraTransform", camera_.GetCameraTransform());
        geometryShader->SetUniform("normalBlend", 1.0f);

        // Bunny is refractive, should not be rendered during deferred rendering.
        ECS& ecs = ECS::Instance();

        ecs.GetComponent<Mesh>(ecs.GetNamedEntityID("Bunny")).Data()->SetActive(false);

        // Render models to FBO attachments.
        ECS::Instance().IterateOver<Transform, Mesh, MaterialCollection>([geometryShader](Transform& transform, Mesh& mesh, MaterialCollection& materialCollection) {
            if (mesh.IsActive()) {
                const glm::mat4& modelTransform = transform.GetMatrix();
                geometryShader->SetUniform("modelTransform", modelTransform);
                geometryShader->SetUniform("normalTransform", glm::transpose(glm::inverse(modelTransform)));

                // Bind all related uniforms with the Phong shader.
                Material* phong = materialCollection.GetNamedMaterial("Phong");
                if (phong) {
                    phong->Bind(geometryShader);
                }

                // Render stage.
                mesh.Bind();
                mesh.Render();
                mesh.Unbind();

                // Post render stage.
            }
        });

        geometryShader->Unbind();

        Backend::Core::DisableFlag(GL_DEPTH_TEST);

        // Render scene depth.
        {
            fbo_.DrawBuffers(5, 1);
            Backend::Core::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT); // Clear everything for a new scene.

            Shader* depthShader = ShaderLibrary::Instance().GetShader("Depth Pass");
            depthShader->Bind();
            depthShader->SetUniform("near", camera_.GetNearPlaneDistance());
            depthShader->SetUniform("far", camera_.GetFarPlaneDistance());
            Backend::Rendering::BindTextureWithSampler(depthShader, fbo_.GetNamedRenderTarget("depth buffer"), "inputTexture", 0);
            Backend::Rendering::DrawFSQ();
            depthShader->Unbind();
        }

        Backend::Core::EnableFlag(GL_DEPTH_TEST);
    }

    void SceneCS562Project5::GlobalLightingPass() {
        // Render to 'output' texture.
        fbo_.DrawBuffers(6, 1);

        Backend::Core::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT); // We are not touching depth buffer here.

        Shader* globalLightingShader = ShaderLibrary::Instance().GetShader("Global Lighting BRDF Pass");
        globalLightingShader->Bind();

        // Set camera uniforms.
        globalLightingShader->SetUniform("cameraPosition", camera_.GetPosition());
        glm::mat4 B = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
        globalLightingShader->SetUniform("shadowTransform", B * CalculateShadowMatrix());
        globalLightingShader->SetUniform("near", camera_.GetNearPlaneDistance());
        globalLightingShader->SetUniform("far", camera_.GetFarPlaneDistance());

        // Set global lighting uniforms.
        globalLightingShader->SetUniform("lightDirection", directionalLight_.direction_);
        globalLightingShader->SetUniform("lightColor", directionalLight_.color_);
        globalLightingShader->SetUniform("lightBrightness", directionalLight_.brightness_);

        // BRDF model.
        globalLightingShader->SetUniform("model", brdfModel_);
        globalLightingShader->SetUniform("exposure", exposure_);
        globalLightingShader->SetUniform("contrast", contrast_);

        // Bind geometry pass textures.
        Backend::Rendering::BindTextureWithSampler(globalLightingShader, fbo_.GetNamedRenderTarget("position"), 0);
        Backend::Rendering::BindTextureWithSampler(globalLightingShader, fbo_.GetNamedRenderTarget("normal"), 1);
        Backend::Rendering::BindTextureWithSampler(globalLightingShader, fbo_.GetNamedRenderTarget("ambient"), 2);
        Backend::Rendering::BindTextureWithSampler(globalLightingShader, fbo_.GetNamedRenderTarget("diffuse"), 3);
        Backend::Rendering::BindTextureWithSampler(globalLightingShader, fbo_.GetNamedRenderTarget("specular"), 4);

        // Bind shadow map.
        shadowMap_.BindForRead();
        if (blurKernelRadius_ > 0) {
            // Vertical blurring pass happens after horizontal blurring pass.
            Backend::Rendering::BindTextureWithSampler(globalLightingShader, shadowMap_.GetNamedRenderTarget("blur vertical"), "shadowMap", 5);
        }
        else {
            Backend::Rendering::BindTextureWithSampler(globalLightingShader, shadowMap_.GetNamedRenderTarget("depth"), "shadowMap", 5);
        }

        Backend::Rendering::BindTextureWithSampler(globalLightingShader, &environmentMap_, "environmentMap", 6);
        Backend::Rendering::BindTextureWithSampler(globalLightingShader, &irradianceMap_, "irradianceMap", 7);

        // Render to output texture using FSQ.
        Backend::Rendering::DrawFSQ();

        globalLightingShader->Unbind();
    }

    void SceneCS562Project5::LocalLightingPass() {
        // Render to 'output' texture.
        fbo_.DrawBuffers(6, 1);

        Shader* localLightingShader = ShaderLibrary::Instance().GetShader("Local Lighting BRDF Pass");
        localLightingShader->Bind();

        localLightingShader->SetUniform("resolution", glm::vec2(fbo_.GetWidth(), fbo_.GetHeight()));

        // Set camera uniforms.
        localLightingShader->SetUniform("cameraPosition", camera_.GetPosition());
        localLightingShader->SetUniform("cameraTransform", camera_.GetCameraTransform());
        localLightingShader->SetUniform("model", brdfModel_);
        localLightingShader->SetUniform("exposure", exposure_);
        localLightingShader->SetUniform("contrast", contrast_);

        // Bind geometry pass textures.
        Backend::Rendering::BindTextureWithSampler(localLightingShader, fbo_.GetNamedRenderTarget("position"), 0);
        Backend::Rendering::BindTextureWithSampler(localLightingShader, fbo_.GetNamedRenderTarget("normal"), 1);
        Backend::Rendering::BindTextureWithSampler(localLightingShader, fbo_.GetNamedRenderTarget("ambient"), 2);
        Backend::Rendering::BindTextureWithSampler(localLightingShader, fbo_.GetNamedRenderTarget("diffuse"), 3);
        Backend::Rendering::BindTextureWithSampler(localLightingShader, fbo_.GetNamedRenderTarget("specular"), 4);

        ECS::Instance().IterateOver<Transform, Mesh, LocalLight>([localLightingShader](Transform& transform, Mesh& mesh, LocalLight& light) {
            localLightingShader->SetUniform("modelTransform", transform.GetMatrix());

            localLightingShader->SetUniform("lightPosition", transform.GetPosition());
            localLightingShader->SetUniform("lightRadius", transform.GetScale().x);
            localLightingShader->SetUniform("lightColor", light.color_);
            localLightingShader->SetUniform("lightBrightness", light.brightness_);

            mesh.Bind();
            mesh.Render();
            mesh.Unbind();
        });

        localLightingShader->Unbind();
    }

    glm::mat4 SceneCS562Project5::CalculateShadowMatrix() {
        // Construct shadow map transformation matrices.
        glm::mat4 projection = camera_.GetPerspectiveTransform();

        static glm::vec3 lightPosition = glm::vec3(0.0f, 8.0f, 0.0f);
        static glm::mat4 rotation = glm::rotate(glm::radians(0.5f * Time::Instance().dt), glm::vec3(0.0f, 1.0f, 0.0f));

        // lightPosition = rotation * glm::vec4(lightPosition, 1.0f);
        glm::vec3 targetPosition = glm::vec3(0.0f);

        directionalLight_.direction_ = glm::normalize(targetPosition - lightPosition);
        directionalLight_.position_ = glm::normalize(lightPosition) * 6.0f;

        glm::mat4 view = glm::lookAt(directionalLight_.position_, directionalLight_.direction_, glm::vec3(0.0f, 0.0f, -1.0f));

        return projection * view;
    }

    void SceneCS562Project5::InitializeBlurKernel() {
        static bool layoutInitialized = false;

        // Initialize uniform block layout.
        const unsigned maxBlurKernelRadius = 50;
        unsigned maximumSize = 2 * maxBlurKernelRadius + 1; // 2 w + 1.

        if (!layoutInitialized) {
            std::vector<UniformBufferElement> layoutElements;
            layoutElements.reserve(maximumSize);

            for (unsigned i = 0; i < maximumSize; ++i) {
                layoutElements.emplace_back(UniformBufferElement { ShaderDataType::FLOAT, "weights[" + std::to_string(i) + "]" } );
            }

            UniformBlockLayout layout { };
            layout.SetBufferElements(0, 1, layoutElements);

            UniformBlock block { 3, layout };
            blurKernel_.SetUniformBlock(block);

            layoutInitialized = true;
        }

        // Provide default values to uniform block data.
        float value = 0.0f;

        const std::vector<UniformBufferElement>& elements = blurKernel_.GetUniformBlock().GetUniformBlockLayout().GetBufferElements();
        for (int i = 0; i < maximumSize; ++i) {
            blurKernel_.SetSubData(elements[i].GetBufferOffset(), 16, static_cast<const void*>(&value));
        }

        unsigned size = 2 * blurKernelRadius_ + 1; // 2 w + 1.

        // Initialize uniform block data.
        std::vector<float> kernel;
        if (blurKernelRadius_ == 0) {
            kernel.resize(size, 1.0f);
        }
        else {
            kernel.resize(size, 0.0f);
        }

        float s = static_cast<float>(blurKernelRadius_) / 2.0f;
        int offset = 0;

        for (int i = -static_cast<int>(blurKernelRadius_); i <= static_cast<int>(blurKernelRadius_); ++i) {
            float p = -(static_cast<float>(i * i)) / (2.0f * static_cast<float>(s * s));
            kernel[offset++] = glm::exp(p);
        }

        // Normalize data to sum to 1.
        // Since set of data is guaranteed to be positive, normalization means dividing each element by the sum.
        float sum = 0.0f;
        for (int i = 0; i < size; ++i) {
            sum += kernel[i];
        }

        if (sum > 0.0f) {
            for (int i = 0; i < size; ++i) {
                kernel[i] /= sum;
            }
        }

        // Set data.
        blurKernel_.Bind();
        for (int i = 0; i < size; ++i) {
            blurKernel_.SetSubData(elements[i].GetBufferOffset(), 16, static_cast<const void*>(&kernel[i]));
        }
        blurKernel_.Unbind();
    }

    // Draws all scene geometry from the perspective of the directional light.
    // Generates 3 textures: depth buffer, output shadow map for visual debugging, and four-channel shadow map for MSM algorithm.
    void SceneCS562Project5::GenerateShadowMap() {
        shadowMap_.DrawBuffers(0, 2);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render scene geometry to generate four-channel depth buffer for MSM algorithm.
        {
            shadowMap_.DrawBuffers(1, 1);

            // Render four channel depth buffer.
            Shader* shadowShader = ShaderLibrary::Instance().GetShader("Shadow Pass");
            shadowShader->Bind();
            shadowShader->SetUniform("shadowTransform", CalculateShadowMatrix());
            shadowShader->SetUniform("near", camera_.GetNearPlaneDistance());
            shadowShader->SetUniform("far", camera_.GetFarPlaneDistance());

            ECS::Instance().IterateOver<Transform, Mesh, ShadowCaster>([shadowShader](Transform& transform, Mesh& mesh, ShadowCaster&) {
                if (mesh.IsActive()) {
                    shadowShader->SetUniform("modelTransform", transform.GetMatrix());

                    mesh.Bind();
                    mesh.Render();
                    mesh.Unbind();
                }
            });

            shadowShader->Unbind();
        }

        Backend::Core::DisableFlag(GL_DEPTH_TEST);

        // Render output shadow map for visual debugging.
        {
            shadowMap_.DrawBuffers(0, 1);

            // Render single channel shadow map out for debug viewing.
            Shader* shadowShader = ShaderLibrary::Instance().GetShader("Depth Out");
            shadowShader->Bind();
            Backend::Rendering::BindTextureWithSampler(shadowShader, shadowMap_.GetNamedRenderTarget("depth"), "inputTexture", 0);
            Backend::Rendering::DrawFSQ();
            shadowShader->Unbind();
        }

        Backend::Core::EnableFlag(GL_DEPTH_TEST);
    }

    void SceneCS562Project5::BlurShadowMap() {
        shadowMap_.DrawBuffers(2, 3);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT);

        Backend::Core::DisableFlag(GL_DEPTH_TEST);

        // Horizontal blur pass.
        {
            Shader* blurShader = ShaderLibrary::Instance().GetShader("Blur Horizontal");

            blurShader->Bind();
            GLuint ID = blurShader->GetID();

            // Set uniforms.
            // 'src' image.
            glBindImageTexture(0, shadowMap_.GetNamedRenderTarget("depth")->ID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
            static GLint src = glGetUniformLocation(ID, "src");
            glUniform1i(src, 0);

            // 'dst' image.
            glBindImageTexture(1, shadowMap_.GetNamedRenderTarget("blur horizontal")->ID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
            static GLint dst = glGetUniformLocation(ID, "dst");
            glUniform1i(dst, 1);

            // 'blurKernel' already set.

            static GLint blurKernelRadius = glGetUniformLocation(ID, "blurKernelRadius");
            glUniform1i(blurKernelRadius, static_cast<int>(blurKernelRadius_));

            // Dispatch shader horizontally.
            glDispatchCompute(shadowMap_.GetWidth() / 128, shadowMap_.GetHeight(), 1);

            blurShader->Unbind();
        }

        // Vertical blur pass.
        {
            Shader* blurShader = ShaderLibrary::Instance().GetShader("Blur Vertical");

            blurShader->Bind();
            GLuint ID = blurShader->GetID();

            // Set uniforms.
            // 'src' image.
            glBindImageTexture(0, shadowMap_.GetNamedRenderTarget("blur horizontal")->ID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
            static GLint src = glGetUniformLocation(ID, "src");
            glUniform1i(src, 0);

            // 'dst' image.
            glBindImageTexture(1, shadowMap_.GetNamedRenderTarget("blur vertical")->ID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
            static GLint dst = glGetUniformLocation(ID, "dst");
            glUniform1i(dst, 1);

            // 'blurKernel' already set.

            static GLint blurKernelRadius = glGetUniformLocation(ID, "blurKernelRadius");
            glUniform1i(blurKernelRadius, static_cast<int>(blurKernelRadius_));

            // Dispatch shader horizontally.
            glDispatchCompute(shadowMap_.GetWidth(), shadowMap_.GetHeight() / 128, 1);

            blurShader->Unbind();
        }

        {
            shadowMap_.DrawBuffers(2, 1);

            Shader* depthShader = ShaderLibrary::Instance().GetShader("Depth Out");
            depthShader->Bind();
            Backend::Rendering::BindTextureWithSampler(depthShader, shadowMap_.GetNamedRenderTarget(blurKernelRadius_ > 0 ? "blur vertical" : "depth"), "depthTexture", 0);
            Backend::Rendering::DrawFSQ();
            depthShader->Unbind();
        }

        Backend::Core::EnableFlag(GL_DEPTH_TEST);
    }

    void SceneCS562Project5::GenerateRandomPoints() {
        // Initialize uniform block layout.
        const int numRandomPoints = 40;

        {
            std::vector<UniformBufferElement> layoutElements;
            layoutElements.reserve(numRandomPoints + 1);

            layoutElements.emplace_back(UniformBufferElement { ShaderDataType::VEC4, "count" } );
            for (unsigned i = 0; i < numRandomPoints; ++i) {
                layoutElements.emplace_back(UniformBufferElement { ShaderDataType::VEC2, "points[" + std::to_string(i) + "]" } );
            }

            UniformBlockLayout layout { };
            layout.SetBufferElements(1, 1, layoutElements);

            UniformBlock block { 4, layout };
            randomPoints_.SetUniformBlock(block);
        }

        {
            // Generate random points (https://en.wikipedia.org/wiki/Low-discrepancy_sequence).
            std::vector<glm::vec2> points;

            float p = 0;
            float u = 0;
            size_t kk = 0;

            for (int k = 0; k < numRandomPoints; k++) {
                for (p = 0.5f, kk = k, u = 0.0f; kk; p *= 0.5f, kk >>= 1)
                    if (kk & 1)
                        u += p;
                float v = ((float)k + 0.5f) / (float)numRandomPoints;
                points.emplace_back(u, v);
            }

            // Set data.
            const std::vector<UniformBufferElement>& layoutElements = randomPoints_.GetUniformBlock().GetUniformBlockLayout().GetBufferElements();
            int index = 0;

            randomPoints_.Bind();
            randomPoints_.SetSubData(layoutElements[index++].GetBufferOffset(), 16, static_cast<const void*>(&numRandomPoints));
            for (const glm::vec2& value : points) {
                randomPoints_.SetSubData(layoutElements[index++].GetBufferOffset(), 16, static_cast<const void*>(&value));
            }
            randomPoints_.Unbind();
        }
    }

    void SceneCS562Project5::InitializeTextures() {
        const std::string environmentMapName = ConvertToNativeSeparators("assets/textures/ibl/monument_valley.hdr");
        const std::string irradianceMapName = ConvertToNativeSeparators(GetAssetDirectory(environmentMapName) + "/" + GetAssetName(environmentMapName) + "_irradiance.hdr");

        environmentMap_.ReserveData(environmentMapName);

        if (!Exists(irradianceMapName)) {
            ImGuiLog::Instance().LogTrace("Generating irradiance map for environment map: '%s'", environmentMapName.c_str());
            GenerateIrradianceMap(environmentMapName);
        }

        irradianceMap_.ReserveData(irradianceMapName);
    }

    void SceneCS562Project5::GenerateIrradianceMap(std::string filename) const {
        filename = ConvertToNativeSeparators(filename);
        std::string name = GetAssetName(filename);
        std::string location = GetAssetDirectory(filename);
        std::string extension = GetAssetExtension(filename);

        if (extension != "hdr") {
            throw std::runtime_error("GenerateIrradianceMap expects an .hdr (HDR) input file.");
        }

        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        // Load raw image data.
        struct HDRImageData {
            void Allocate() {
                if (width < 0 || height < 0 || channels < 0) {
                    throw std::runtime_error("HDRImageData dimensions not initialized.");
                }
                data = new float[width * height * channels];

                for (int i = 0; i < width * height * channels; ++i) {
                    data[i] = 0.0f;
                }
            }

            void Deallocate(bool stbi) const {
                stbi ? stbi_image_free(data) : delete[] data;
            }

            int width = -1;
            int height = -1;
            int channels = -1;

            float* data = nullptr;
        };

        HDRImageData in { };
        in.data = stbi_loadf(filename.c_str(), &in.width, &in.height, &in.channels, 0);
        if (!in.data) {
            throw std::runtime_error("Failed to read .hdr (HDR) image (" + filename + ").");
        }

        // Set 1: (Analytic) Projection of the dot product term.
        std::array<float, 3> A = { glm::pi<float>(), (2.0f / 3.0f) * glm::pi<float>(), (1.0f / 4.0f) * glm::pi<float>() };

        // Set 2: Projection of the pixels of the input image.
        // Y - nine spherical harmonic basis functions (first three bands).
        static std::array<std::vector<float (*)(float, float, float)>, 3> Y; // Constant, linear, quadratic.
        static bool initialized = false;

        if (!initialized) {
            int constant = 0;
            int linear = 1;
            int quadratic = 2;

            // Constant.
            Y[constant].emplace_back(+[](float x, float y, float z) -> float {
                return (1.0f / 2.0f) * glm::sqrt(1.0f / glm::pi<float>());
            });

            // Linear.
            Y[linear].emplace_back(+[](float x, float y, float z) -> float {
                return (1.0f / 2.0f) * glm::sqrt(3.0f / glm::pi<float>()) * y;
            });
            Y[linear].emplace_back(+[](float x, float y, float z) -> float {
                return (1.0f / 2.0f) * glm::sqrt(3.0f / glm::pi<float>()) * z;
            });
            Y[linear].emplace_back(+[](float x, float y, float z) -> float {
                return (1.0f / 2.0f) * glm::sqrt(3.0f / glm::pi<float>()) * x;
            });

            // Quadratic.
            Y[quadratic].emplace_back(+[](float x, float y, float z) -> float {
                return (1.0f / 2.0f) * glm::sqrt(15.0f / glm::pi<float>()) * x * y;
            });
            Y[quadratic].emplace_back(+[](float x, float y, float z) -> float {
                return (1.0f / 2.0f) * glm::sqrt(15.0f / glm::pi<float>()) * y * z;
            });
            Y[quadratic].emplace_back(+[](float x, float y, float z) -> float {
                return (1.0f / 4.0f) * glm::sqrt(5.0f / glm::pi<float>()) * (3.0f * (z * z) - 1.0f);
            });
            Y[quadratic].emplace_back(+[](float x, float y, float z) -> float {
                return (1.0f / 2.0f) * glm::sqrt(15.0f / glm::pi<float>()) * x * z;
            });
            Y[quadratic].emplace_back(+[](float x, float y, float z) -> float {
                return (1.0f / 4.0f) * glm::sqrt(15.0f / glm::pi<float>()) * ((x * x) - (y * y));
            });

            initialized = true;
        }

        std::array<std::vector<glm::vec3>, 3> L; // RGB coefficients, one for each harmonic basis function of Y.

        float dTheta = glm::pi<float>() / static_cast<float>(in.height);
        float dPhi = 2.0f * glm::pi<float>() / static_cast<float>(in.width);

        for (int j = 0; j < in.height; ++j) {
            for (int i = 0; i < in.width; ++i) {
                int index = ((in.width * j) + i) * in.channels;
                glm::vec3 rgb = glm::vec3(in.data[index + 0], in.data[index + 1], in.data[index + 2]);

                // +0.5 to compute the center of the corresponding point on the sphere.
                float theta = glm::pi<float>() * (static_cast<float>(j) + 0.5f) / static_cast<float>(in.height);
                float phi = 2.0f * glm::pi<float>() * (static_cast<float>(i) + 0.5f) / static_cast<float>(in.width);

                float x = glm::sin(theta) * glm::cos(phi);
                float y = glm::sin(theta) * glm::sin(phi);
                float z = glm::cos(theta);

                float sinTheta = glm::sin(theta);

                for (int l = 0; l < Y.size(); ++l) {
                    L[l].resize(Y[l].size()); // Number of coefficients matches the number of harmonic basis functions at index 'l'.
                    for (int m = 0; m < Y[l].size(); ++m) {
                        // RGB pixel value * basis function evaluation at (x, y, z) * sine function * step-size deltas.
                        L[l][m] += rgb * Y[l][m](x, y, z) * sinTheta * dTheta * dPhi;
                    }
                }
            }
        }

        in.Deallocate(true);

        HDRImageData out { };
        // Native resolution.
        out.width = in.width;
        out.height = in.height;
        out.channels = 3;
        out.Allocate();

        // Set 3: Final set of harmonic coefficients comes from the product of the above sets.
        for (int j = 0; j < out.height; ++j) {
            for (int i = 0; i < out.width; ++i) {
                int index = ((out.width * j) + i) * out.channels;

                float theta = glm::pi<float>() * (static_cast<float>(j) + 0.5f) / static_cast<float>(out.height);
                float phi = 2.0f * glm::pi<float>() * (static_cast<float>(i) + 0.5f) / static_cast<float>(out.width);

                float x = glm::sin(theta) * glm::cos(phi);
                float y = glm::sin(theta) * glm::sin(phi);
                float z = glm::cos(theta);

                // Computing irradiance at pixel (x, y, z).
                glm::vec3 irradiance = glm::vec3(0.0f);
                for (int l = 0; l < Y.size(); ++l) {
                    for (int m = 0; m < Y[l].size(); ++m) {
                        irradiance += (A[l] * L[l][m]) * Y[l][m](x, y, z);
                    }
                }

                out.data[index + 0] = irradiance.r;
                out.data[index + 1] = irradiance.g;
                out.data[index + 2] = irradiance.b;
            }
        }

        stbi_write_hdr(ConvertToNativeSeparators(location + "/" + name + "_irradiance.hdr").c_str(), out.width, out.height, out.channels, out.data);
        out.Deallocate(false);

        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration t = end - start;
        std::chrono::duration ms = std::chrono::duration_cast<std::chrono::milliseconds>(t);
        std::chrono::duration s = std::chrono::duration_cast<std::chrono::seconds>(t);

        ImGuiLog::Instance().LogTrace("Generating irradiance map '%s' at resolution %i x %i (native resolution %i x %i) took %i milliseconds (%i seconds).", filename.c_str(), out.width, out.height, in.width, in.height, ms.count(), s.count());
    }

    void SceneCS562Project5::RenderSkydome() {
        fbo_.DrawBuffers(6, 1);

        // Render four channel depth buffer.
        Shader* skydomeShader = ShaderLibrary::Instance().GetShader("Skydome");
        skydomeShader->Bind();

        glm::mat4 view = glm::mat4(glm::mat3(camera_.GetViewTransform())); // Remove translation.
        glm::mat4 perspective = camera_.GetPerspectiveTransform();

        skydomeShader->SetUniform("cameraTransform", perspective * view);
        skydomeShader->SetUniform("exposure", exposure_);
        skydomeShader->SetUniform("contrast", contrast_);
        Backend::Rendering::BindTextureWithSampler(skydomeShader, &environmentMap_, "environmentMap", 0);

        ECS::Instance().IterateOver<Transform, Mesh, Skydome>([skydomeShader](Transform& transform, Mesh& mesh, Skydome&) {
            skydomeShader->SetUniform("modelTransform", transform.GetMatrix());
            skydomeShader->SetUniform("normalTransform", glm::inverse(glm::transpose(transform.GetMatrix())));

            mesh.Bind();
            mesh.Render();
            mesh.Unbind();
        });

        skydomeShader->Unbind();
    }

    void SceneCS562Project5::ConstructRefractionFBO() {
        int contentWidth = 2048;
        int contentHeight = 2048;

//        // Generate cube map texture.
//        glGenTextures(1, &refractionCubemap_);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, refractionCubemap_);
//        glTexImage2D(GL_TEXTURE_CUBE_MAP, 0, GL_RGBA32F, 2048, 2048, 0, GL_RGBA, GL_FLOAT, nullptr);
//        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
//
//        glGenFramebuffers(1, &refractionFBO_);
//        glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO_);
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTarget->ID(), 0);


        refractionFBO_.BindForReadWrite();

        // Textures for cubemap faces.
        Texture* posX = new Texture("posX");
        posX->Bind();
        posX->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        posX->Unbind();
        refractionFBO_.AttachRenderTarget(posX);

        Texture* negX = new Texture("negX");
        negX->Bind();
        negX->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        negX->Unbind();
        refractionFBO_.AttachRenderTarget(negX);

        Texture* posY = new Texture("posY");
        posY->Bind();
        posY->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        posY->Unbind();
        refractionFBO_.AttachRenderTarget(posY);

        Texture* negY = new Texture("negY");
        negY->Bind();
        negY->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        negY->Unbind();
        refractionFBO_.AttachRenderTarget(negY);

        Texture* posZ = new Texture("posZ");
        posZ->Bind();
        posZ->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        posZ->Unbind();
        refractionFBO_.AttachRenderTarget(posZ);

        Texture* negZ = new Texture("negZ");
        negZ->Bind();
        negZ->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        negZ->Unbind();
        refractionFBO_.AttachRenderTarget(negZ);

        // Depth buffer (RBO).
        RenderBufferObject* depthBuffer = new RenderBufferObject();
        depthBuffer->Bind();
        depthBuffer->ReserveData(contentWidth, contentHeight);
        refractionFBO_.AttachDepthBuffer(depthBuffer);

        if (!refractionFBO_.CheckStatus()) {
            throw std::runtime_error("Custom Shadow FBO is not complete.");
        }

        refractionFBO_.Unbind();
    }

    void SceneCS562Project5::ConstructCausticFBO() {
        int contentWidth = causticFBO_.GetWidth();
        int contentHeight = causticFBO_.GetHeight();

        // Receiver positions attachment.
        causticFBO_.BindForReadWrite();

        Texture* receiverPositions = new Texture("receiver positions");
        receiverPositions->Bind();
        receiverPositions->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        receiverPositions->Unbind();
        causticFBO_.AttachRenderTarget(receiverPositions);

        Texture* refractivePositions = new Texture("refractive positions");
        refractivePositions->Bind();
        refractivePositions->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        refractivePositions->Unbind();
        causticFBO_.AttachRenderTarget(refractivePositions);

        Texture* refractiveNormals = new Texture("refractive normals");
        refractiveNormals->Bind();
        refractiveNormals->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        refractiveNormals->Unbind();
        causticFBO_.AttachRenderTarget(refractiveNormals);

        Texture* output = new Texture("output");
        output->Bind();
        output->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        output->Unbind();
        causticFBO_.AttachRenderTarget(output);

        // Depth buffer (RBO).
        RenderBufferObject* depthBuffer = new RenderBufferObject();
        depthBuffer->Bind();
        depthBuffer->ReserveData(contentWidth, contentHeight);
        causticFBO_.AttachDepthBuffer(depthBuffer);

        if (!causticFBO_.CheckStatus()) {
            throw std::runtime_error("Custom Caustic FBO is not complete.");
        }

        causticFBO_.Unbind();
    }

    void SceneCS562Project5::GenerateCubemap() {
        static const int POSX = 0;
        static const int NEGX = 1;
        static const int POSY = 2;
        static const int NEGY = 3;
        static const int POSZ = 4;
        static const int NEGZ = 5;

        static const std::vector<int> faces = { POSX, NEGX, POSY, NEGY, POSZ, NEGZ };

        // Clear all buffers.
        refractionFBO_.BindForReadWrite();

        refractionFBO_.DrawBuffers();
        Backend::Core::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        FPSCamera camera = camera_;
        camera.SetAspectRatio(1.0f);
        camera.SetFOVAngle(90.0f);

        ECS& ecs = ECS::Instance();
        int bunnyID = ecs.GetNamedEntityID("Bunny");
        camera.SetPosition(ecs.GetComponent<Transform>(bunnyID)->GetPosition());

        // Disable main refractive model.
        assert(ecs.HasComponent<Refractive>(bunnyID));
        ecs.GetComponent<Mesh>(bunnyID).Data()->SetActive(false);

        for (int face : faces) {
            switch (face) {
                case POSX: {
                    camera.SetLookAtDirection(glm::vec3(1.0f, 0.0f, 0.0f));
                    camera.SetUpVector(glm::vec3(0.0f, 1.0f, 0.0f));
                    break;
                }

                case NEGX: {
                    camera.SetLookAtDirection(glm::vec3(-1.0f, 0.0f, 0.0f));
                    camera.SetUpVector(glm::vec3(0.0f, 1.0f, 0.0f));
                    break;
                }

                case POSY: {
                    camera.SetLookAtDirection(glm::vec3(0.0f, 1.0f, 0.0f));
                    camera.SetUpVector(glm::vec3(0.0f, 0.0f, -1.0f));
                    break;
                }

                case NEGY: {
                    camera.SetLookAtDirection(glm::vec3(0.0f, -1.0f, 0.0f));
                    camera.SetUpVector(glm::vec3(0.0f, 0.0f, 1.0f));
                    break;
                }

                case POSZ: {
                    camera.SetLookAtDirection(glm::vec3(0.0f, 0.0f, 1.0f));
                    camera.SetUpVector(glm::vec3(0.0f, 1.0f, 0.0f));
                    break;
                }

                case NEGZ: {
                    camera.SetLookAtDirection(glm::vec3(0.0f, 0.0f, -1.0f));
                    camera.SetUpVector(glm::vec3(0.0f, 1.0f, 0.0f));
                    break;
                }

                default:
                    assert(false); // Oops.
            }

            refractionFBO_.DrawBuffers(face, 1);
            RenderScene(camera);
            Backend::Core::ClearFlag(GL_DEPTH_BUFFER_BIT); // Clear depth buffer for next attachment.
        }

        // Return to normal.
        ecs.GetComponent<Mesh>(bunnyID).Data()->SetActive(true);

//        // Construct cube map.
//        unsigned width = refractionFBO_.GetWidth();
//        unsigned height = refractionFBO_.GetHeight();
//
//        glBindTexture(GL_TEXTURE_CUBE_MAP, refractionCubemap_);
//
//        for (int face : faces) {
//            Texture* attachment = nullptr;
//
//            switch (face) {
//                case POSX: {
//                    attachment = refractionFBO_.GetNamedRenderTarget("posX");
//                    break;
//                }
//
//                case NEGX: {
//                    attachment = refractionFBO_.GetNamedRenderTarget("negX");
//                    break;
//                }
//
//                case POSY: {
//                    attachment = refractionFBO_.GetNamedRenderTarget("posY");
//                    break;
//                }
//
//                case NEGY: {
//                    attachment = refractionFBO_.GetNamedRenderTarget("negY");
//                    break;
//                }
//
//                case POSZ: {
//                    attachment = refractionFBO_.GetNamedRenderTarget("posZ");
//                    break;
//                }
//
//                case NEGZ: {
//                    attachment = refractionFBO_.GetNamedRenderTarget("negZ");
//                    break;
//                }
//
//                default:
//                    assert(false);
//            }
//
//            glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment->GetAttachmentLocation());
//            glCopyTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA16, 0, 0, width, height, 0);
//        }
//
//        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        refractionFBO_.Unbind();
    }

    void SceneCS562Project5::RenderScene(FPSCamera camera) {
        // Render scene geometry, fully lit by BRDF lighting.
        {
            Shader* shader = ShaderLibrary::Instance().GetShader("Global Lighting BRDF Forward Pass");

            shader->Bind();

            // Bind shadow map.
            shadowMap_.BindForRead();
            if (blurKernelRadius_ > 0) {
                // Vertical blurring pass happens after horizontal blurring pass.
                Backend::Rendering::BindTextureWithSampler(shader, shadowMap_.GetNamedRenderTarget("blur vertical"), "shadowMap", 0);
            }
            else {
                Backend::Rendering::BindTextureWithSampler(shader, shadowMap_.GetNamedRenderTarget("depth"), "shadowMap", 0);
            }

            Backend::Rendering::BindTextureWithSampler(shader, &environmentMap_, "environmentMap", 1);
            Backend::Rendering::BindTextureWithSampler(shader, &irradianceMap_, "irradianceMap", 2);

            shader->SetUniform("model", brdfModel_);
            shader->SetUniform("exposure", exposure_);
            shader->SetUniform("contrast", contrast_);
            shader->SetUniform("cameraPosition", camera.GetPosition());
            glm::mat4 B = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
            shader->SetUniform("shadowTransform", B * CalculateShadowMatrix());
            shader->SetUniform("near", camera.GetNearPlaneDistance());
            shader->SetUniform("far", camera.GetFarPlaneDistance());
            shader->SetUniform("lightDirection", directionalLight_.direction_);
            shader->SetUniform("lightColor", directionalLight_.color_);
            shader->SetUniform("lightBrightness", directionalLight_.brightness_);
            shader->SetUniform("cameraTransform", camera.GetCameraTransform());

            ECS::Instance().IterateOver<Transform, Mesh, MaterialCollection, ShadowCaster>([shader](Transform& transform, Mesh& mesh, MaterialCollection& materialCollection, ShadowCaster&) {
                if (!mesh.IsActive()) {
                    return;
                }

                const glm::mat4& modelTransform = transform.GetMatrix();
                shader->SetUniform("modelTransform", modelTransform);
                shader->SetUniform("normalTransform", glm::transpose(glm::inverse(modelTransform)));

                // Bind all related uniforms with the Phong shader.
                Material* phong = materialCollection.GetNamedMaterial("Phong");
                if (phong) {
                    phong->Bind(shader);
                }

                mesh.Bind();
                mesh.Render();
                mesh.Unbind();
            });

            shader->Unbind();
        }

        // TODO: Local pass.

        // Render skydome.
        {
            Shader* shader = ShaderLibrary::Instance().GetShader("Skydome");
            shader->Bind();

            glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewTransform())); // Remove translation.
            glm::mat4 perspective = camera.GetPerspectiveTransform();

            shader->SetUniform("cameraTransform", perspective * view);
            shader->SetUniform("exposure", exposure_);
            shader->SetUniform("contrast", contrast_);
            Backend::Rendering::BindTextureWithSampler(shader, &environmentMap_, "environmentMap", 0);

            ECS::Instance().IterateOver<Transform, Mesh, Skydome>([shader](Transform& transform, Mesh& mesh, Skydome&) {
                if (mesh.IsActive()) {
                    const glm::mat4 modelTransform = transform.GetMatrix();

                    shader->SetUniform("modelTransform", modelTransform);
                    shader->SetUniform("normalTransform", glm::inverse(glm::transpose(modelTransform)));

                    mesh.Bind();
                    mesh.Render();
                    mesh.Unbind();
                }
            });

            shader->Unbind();
        }
    }

    void SceneCS562Project5::RenderRefractiveObject() {
        Shader* shader = ShaderLibrary::Instance().GetShader("Environment Mapping");
        shader->Bind();

        shader->SetUniform("cameraTransform", camera_.GetCameraTransform());
        shader->SetUniform("cameraPosition", camera_.GetPosition());

        // Bind samplers.
        refractionFBO_.BindForRead();
        Backend::Rendering::BindTextureWithSampler(shader, refractionFBO_.GetNamedRenderTarget("posX"), 0);
        Backend::Rendering::BindTextureWithSampler(shader, refractionFBO_.GetNamedRenderTarget("negX"), 1);
        Backend::Rendering::BindTextureWithSampler(shader, refractionFBO_.GetNamedRenderTarget("posY"), 2);
        Backend::Rendering::BindTextureWithSampler(shader, refractionFBO_.GetNamedRenderTarget("negY"), 3);
        Backend::Rendering::BindTextureWithSampler(shader, refractionFBO_.GetNamedRenderTarget("posZ"), 4);
        Backend::Rendering::BindTextureWithSampler(shader, refractionFBO_.GetNamedRenderTarget("negZ"), 5);

        ECS::Instance().IterateOver<Transform, Mesh, Refractive>([shader](Transform& transform, Mesh& mesh, Refractive& refractive) {
            const glm::mat4 modelTransform = transform.GetMatrix();

            shader->SetUniform("modelTransform", modelTransform);
            shader->SetUniform("normalTransform", glm::inverse(glm::transpose(modelTransform)));
            shader->SetUniform("ior", refractive.ior);

            mesh.Bind();
            mesh.Render();
            mesh.Unbind();
        });

        shader->Unbind();
    }

    void SceneCS562Project5::CausticMapping() {
        causticFBO_.DrawBuffers();
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render receiver geometry from the position of the light.
        {
            causticFBO_.DrawBuffers(0, 1);
            Backend::Core::ClearFlag(GL_DEPTH_BUFFER_BIT);

            Shader* shader = ShaderLibrary::Instance().GetShader("Caustic Receiver");
            shader->Bind();
            shader->SetUniform("cameraTransform", CalculateShadowMatrix());

            // Disable refractive model.
            ECS::Instance().GetComponent<Mesh>(ECS::Instance().GetNamedEntityID("Bunny"))->SetActive(false);

            ECS::Instance().IterateOver<Transform, Mesh>([shader](Transform& transform, Mesh& mesh) {
                if (mesh.IsActive()) {
                    const glm::mat4& modelTransform = transform.GetMatrix();

                    shader->SetUniform("modelTransform", modelTransform);
                    shader->SetUniform("normalTransform", glm::transpose(glm::inverse(modelTransform)));

                    mesh.Bind();
                    mesh.Render();
                    mesh.Unbind();
                }
            });

            shader->Unbind();
        }

        {
            causticFBO_.DrawBuffers(1, 2);
            Backend::Core::ClearFlag(GL_DEPTH_BUFFER_BIT);

            Shader* shader = ShaderLibrary::Instance().GetShader("Caustic Refractive");
            shader->Bind();
            shader->SetUniform("cameraTransform", CalculateShadowMatrix());

            // Disable refractive model.
            ECS::Instance().GetComponent<Mesh>(ECS::Instance().GetNamedEntityID("Bunny"))->SetActive(true);

            ECS::Instance().IterateOver<Transform, Mesh, Refractive>([shader](Transform& transform, Mesh& mesh, Refractive&) {
                if (mesh.IsActive()) {
                    const glm::mat4& modelTransform = transform.GetMatrix();

                    shader->SetUniform("modelTransform", modelTransform);
                    shader->SetUniform("normalTransform", glm::transpose(glm::inverse(modelTransform)));
                    shader->SetUniform("normalBlend", 1.0f);

                    mesh.Bind();
                    mesh.Render();
                    mesh.Unbind();
                }
            });

            shader->Unbind();
        }

        // 3. Local lighting pass.
        Backend::Core::EnableFlag(GL_BLEND); // Enable blending.
        glBlendFunc(GL_ONE, GL_ONE);         // Additive blending

        {
            causticFBO_.DrawBuffers(3, 1);
            Backend::Core::ClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Shader* shader = ShaderLibrary::Instance().GetShader("Caustic Mapping");
            shader->Bind();
            shader->SetUniform("cameraTransform", camera_.GetCameraTransform());
            // shader->SetUniform("cameraPosition", camera_.GetPosition());
            shader->SetUniform("lightPosition", directionalLight_.position_);
            // glm::mat4 view = glm::lookAt(glm::normalize(glm::vec3(0.0f, 8.0f, 0.0f)) * 6.0f, directionalLight_.direction_, glm::vec3(0.0f, 0.0f, -1.0f));
            shader->SetUniform("viewTransform", CalculateShadowMatrix());

            Backend::Rendering::BindTextureWithSampler(shader, causticFBO_.GetNamedRenderTarget("receiver positions"), "receiver", 0);
            Backend::Rendering::BindTextureWithSampler(shader, causticFBO_.GetNamedRenderTarget("refractive positions"), "refractivePositions", 1);
            Backend::Rendering::BindTextureWithSampler(shader, causticFBO_.GetNamedRenderTarget("refractive normals"), "refractiveNormals", 2);

//            glEnable(GL_POINT_SMOOTH);
//            glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
            glPointSize(10);

            if (vertexGrid_.IsActive()) {
                shader->SetUniform("modelTransform", glm::mat4(1.0f));
                shader->SetUniform("ior", ECS::Instance().GetComponent<Refractive>(ECS::Instance().GetNamedEntityID("Bunny"))->ior);

                vertexGrid_.Bind();
                vertexGrid_.Render();
                vertexGrid_.Unbind();
            }

            shader->Unbind();
        }

        Backend::Core::DisableFlag(GL_BLEND); // Disable blending.
    }

    void SceneCS562Project5::InitializeVertexGrid() {
        int dimension = 256; // causticFBO_.GetWidth();

        std::vector<glm::vec3> vertices;

        float start = -1.0f;
        float step = 2.0f / static_cast<float>(dimension);

        for (int i = 0; i < dimension; ++i) {
            for (int j = 0; j < dimension; ++j) {
                vertices.emplace_back(glm::vec3(start + static_cast<float>(i) * step, start + static_cast<float>(j) * step, 0.0f) * 10.0f);
            }
        }

        std::vector<unsigned> indices;
        for (unsigned i = 0; i < dimension * dimension; ++i) {
            indices.emplace_back(i);
        }

        vertexGrid_.SetVertices(vertices);
        vertexGrid_.SetIndices(indices, MeshTopology::POINTS);
        vertexGrid_.Complete();
    }

}