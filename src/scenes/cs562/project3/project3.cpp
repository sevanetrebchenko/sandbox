
#include "scenes/cs562/project3/project3.h"
#include "common/api/window.h"
#include "common/geometry/object_loader.h"
#include "common/utility/log.h"
#include "common/geometry/transform.h"
#include "common/ecs/ecs.h"
#include "common/application/time.h"

namespace Sandbox {

    SceneCS562Project3::SceneCS562Project3() : fbo_(2560, 1440),
                                               shadowMap_(2048, 2048),
                                               camera_(Window::Instance().GetWidth(), Window::Instance().GetHeight()),
                                               blurKernelRadius_(25)
                                               {
    }

    SceneCS562Project3::~SceneCS562Project3() {
    }

    void SceneCS562Project3::OnInit() {
        IScene::OnInit();

        InitializeShaders();
        InitializeMaterials();

        ConfigureLights();
        ConfigureModels();
        ConstructFBO();
        ConstructShadowMap();
        InitializeBlurKernel();
        GenerateRandomPoints();

        camera_.SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    }

    void SceneCS562Project3::OnUpdate() {
        IScene::OnUpdate();
        camera_.Update();

        // Rotate center model.
//        ComponentWrapper<Transform> transform = ECS::Instance().GetComponent<Transform>("Bunny");
//        transform->SetRotation(transform->GetRotation() + glm::vec3(0.0f, 10.0f, 0.0f) * Time::Instance().dt);
    }

    void SceneCS562Project3::OnPreRender() {
        IScene::OnPreRender();
    }

    void SceneCS562Project3::OnRender() {
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

        // Set viewport.
        fbo_.BindForReadWrite();
        Backend::Core::SetViewport(0, 0, fbo_.GetWidth(), fbo_.GetHeight());

        // 1. G-buffer pass.
        GeometryPass();

        Backend::Core::DisableFlag(GL_DEPTH_TEST);

        // 2. Lighting pass.
        GlobalLightingPass();

        // 3. Local lighting pass.
        Backend::Core::EnableFlag(GL_BLEND); // Enable blending.
        glBlendFunc(GL_ONE, GL_ONE);         // Additive blending TODO: abstract out.

        Backend::Core::EnableFlag(GL_CULL_FACE);
        Backend::Core::CullFace(GL_FRONT);

        LocalLightingPass();

        Backend::Core::DisableFlag(GL_CULL_FACE);
        Backend::Core::DisableFlag(GL_BLEND);
        Backend::Core::EnableFlag(GL_DEPTH_TEST);

        // Restore viewport.
        fbo_.Unbind();
        Backend::Core::SetViewport(viewport.x, viewport.y, viewport.z, viewport.w);
    }

    void SceneCS562Project3::OnPostRender() {
        IScene::OnPostRender();
    }

    void SceneCS562Project3::OnImGui() {
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
                fbo_.SaveRenderTargetsToDirectory("data/scenes/cs562_project_2/");
                shadowMap_.SaveRenderTargetsToDirectory("data/scenes/cs562_project_2/");
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

        // Draw individual deferred rendering textures.
        static bool showDebugTextures = false;
        if (ImGui::Begin("Debug Textures"), &showDebugTextures) {
            {
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

            {
                Texture* shadowMap = shadowMap_.GetNamedRenderTarget("depth output");
                float maxWidth = ImGui::GetWindowContentRegionWidth();
                ImVec2 imageSize = ImVec2(maxWidth, maxWidth / (static_cast<float>(shadowMap->GetWidth()) / static_cast<float>(shadowMap->GetHeight())));

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
                float maxWidth = ImGui::GetWindowContentRegionWidth();
                ImVec2 imageSize = ImVec2(maxWidth, maxWidth / (static_cast<float>(shadowMap->GetWidth()) / static_cast<float>(shadowMap->GetHeight())));

                ImGui::Text("Blurred Shadow Map:");
                ImGui::Image(reinterpret_cast<ImTextureID>(shadowMap->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            }
        }
        ImGui::End();

        ImGuiLog::Instance().OnImGui();
    }

    void SceneCS562Project3::OnShutdown() {
        IScene::OnShutdown();
    }

    void SceneCS562Project3::OnWindowResize(int width, int height) {
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

    void SceneCS562Project3::InitializeShaders() {
        ShaderLibrary& shaderLibrary = ShaderLibrary::Instance();

        shaderLibrary.CreateShader("Geometry Pass", { "assets/shaders/geometry_buffer.vert", "assets/shaders/geometry_buffer.frag" });
        shaderLibrary.CreateShader("Global Lighting BRDF Pass", { "assets/shaders/global_brdf.vert", "assets/shaders/global_brdf.frag" });
        // shaderLibrary.CreateShader("Local Lighting BRDF Pass", { "assets/shaders/local_brdf.vert", "assets/shaders/local_brdf.frag" });
        shaderLibrary.CreateShader("FSQ", { "assets/shaders/fsq.vert", "assets/shaders/fsq.frag" });

        shaderLibrary.CreateShader("Shadow Pass", { "assets/shaders/shadow.vert", "assets/shaders/shadow.frag" });
        shaderLibrary.CreateShader("Depth Pass", { "assets/shaders/fsq.vert", "assets/shaders/depth.frag" });
        shaderLibrary.CreateShader("Depth Out", { "assets/shaders/fsq.vert", "assets/shaders/depth_out.frag" });
        shaderLibrary.CreateShader("Blur Horizontal", { "assets/shaders/blur_horizontal.comp" });
        shaderLibrary.CreateShader("Blur Vertical", { "assets/shaders/blur_vertical.comp" });
    }

    void SceneCS562Project3::InitializeMaterials() {
        // Phong shading material.
        Material* phongMaterial = new Material("Phong", {
                { "ambientCoefficient", glm::vec3(0.5f) },
                { "diffuseCoefficient", glm::vec3(0.5f) },
                { "specularCoefficient", glm::vec3(1.0f) },
                { "specularExponent", 50.0f }
        });
        phongMaterial->GetUniform("ambientCoefficient")->UseColorPicker(true);
        phongMaterial->GetUniform("diffuseCoefficient")->UseColorPicker(true);
        phongMaterial->GetUniform("specularCoefficient")->UseColorPicker(true);
        phongMaterial->GetUniform("specularExponent")->SetSliderRange(0.0f, 100.0f);
        materialLibrary_.AddMaterial(phongMaterial);
    }

    void SceneCS562Project3::ConfigureModels() {
        ECS& ecs = ECS::Instance();

        // Bunny.
        int bunny = ecs.CreateEntity("Bunny");

        ecs.AddComponent<Mesh>(bunny, OBJLoader::Instance().LoadFromFile(OBJLoader::Request("assets/models/bunny_high_poly.obj"))).Configure([](Mesh& mesh) {
            mesh.Complete();
        });

        ecs.AddComponent<MaterialCollection>(bunny).Configure([this](MaterialCollection& materialCollection) {
            Material* phong = materialLibrary_.GetMaterialInstance("Phong");
            phong->GetUniform("ambientCoefficient")->SetData(glm::vec3(0.05f));
            phong->GetUniform("diffuseCoefficient")->SetData(glm::vec3(0.8f));
            phong->GetUniform("specularCoefficient")->SetData(glm::vec3(1.0f));
            phong->GetUniform("specularExponent")->SetData(50.0f);

            materialCollection.SetMaterial(phong);
        });

        // Floor.
        int floor = ecs.CreateEntity("Floor");
        ecs.AddComponent<Mesh>(floor, OBJLoader::Instance().LoadFromFile(OBJLoader::Request("assets/models/quad.obj"))).Configure([](Mesh& mesh) {
            mesh.Complete();
        });

        ecs.AddComponent<MaterialCollection>(floor).Configure([this](MaterialCollection& materialCollection) {
            Material* phong = materialLibrary_.GetMaterialInstance("Phong");
            phong->GetUniform("ambientCoefficient")->SetData(glm::vec3(0.2f));
            phong->GetUniform("diffuseCoefficient")->SetData(glm::vec3(0.1f));
            phong->GetUniform("specularCoefficient")->SetData(glm::vec3(0.8f));
            phong->GetUniform("specularExponent")->SetData(14.0f);

            materialCollection.SetMaterial(phong);
        });

        ecs.GetComponent<Transform>(floor).Configure([](Transform& transform) {
            transform.SetPosition(glm::vec3(0.0f, -2.0f, 0.0f));
            transform.SetScale(glm::vec3(5.0f));
            transform.SetRotation(glm::vec3(270.0f, 0.0f, 0.0f));
        });

        // Compute scene bounds.
        ecs.IterateOver<Transform, Mesh>([this](Transform& transform, Mesh& mesh) {
            // Compute object bounds.
            std::vector<glm::vec3> vertices = mesh.GetVertices();
            glm::mat4 matrix = transform.GetMatrix();

            for (const glm::vec3& vertex : vertices) {
                bounds_.Extend(matrix * glm::vec4(vertex, 1.0f));
            }
        });
    }

    void SceneCS562Project3::ConfigureLights() {
        directionalLight_.brightness_ = 1.0f;

//        ECS& ecs = ECS::Instance();
//        Mesh mesh = OBJLoader::Instance().LoadFromFile(OBJLoader::Request("assets/models/sphere.obj"));
//
//        int ID = ecs.CreateEntity("light");
//        ecs.AddComponent<Mesh>(ID, mesh).Configure([](Mesh& mesh) {
//            mesh.Complete();
//        });
//
//        ecs.GetComponent<Transform>(ID).Configure([](Transform& transform) {
//            transform.SetPosition(glm::vec3(0.0f, 10.0f, 0.0f));
//            transform.SetScale(glm::vec3(20.f));
//        });
//        ecs.AddComponent<LocalLight>(ID, glm::vec3(1.0f), 4.0f);
    }

    void SceneCS562Project3::ConstructFBO() {
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

    void SceneCS562Project3::ConstructShadowMap() {
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

    void SceneCS562Project3::GeometryPass() {
        fbo_.DrawBuffers(0, 5);
        Backend::Core::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear everything for a new scene.

        Shader* geometryShader = ShaderLibrary::Instance().GetShader("Geometry Pass");
        geometryShader->Bind();

        // Set camera uniforms.
        geometryShader->SetUniform("cameraTransform", camera_.GetCameraTransform());
        geometryShader->SetUniform("normalBlend", 1.0f);

        // Render models to FBO attachments.
        ECS::Instance().IterateOver<Transform, Mesh, MaterialCollection>([geometryShader](Transform& transform, Mesh& mesh, MaterialCollection& materialCollection) {
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

    void SceneCS562Project3::GlobalLightingPass() {
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
        const int phong = 0;
        const int ggx = 1;
        const int beckman = 2;
        globalLightingShader->SetUniform("model", phong);

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

        // Render to output texture using FSQ.
        Backend::Rendering::DrawFSQ();

        globalLightingShader->Unbind();
    }

    void SceneCS562Project3::LocalLightingPass() {
//        // Render to 'output' texture.
//        fbo_.DrawBuffers(6, 1);
//        // No clearing here.
//
//        Shader* localLightingShader = ShaderLibrary::Instance().GetShader("Local Lighting Pass");
//        localLightingShader->Bind();
//
//        localLightingShader->SetUniform("resolution", glm::vec2(fbo_.GetWidth(), fbo_.GetHeight()));
//
//        // Set camera uniforms.
//        localLightingShader->SetUniform("cameraPosition", camera_.GetPosition());
//        localLightingShader->SetUniform("cameraTransform", camera_.GetCameraTransform());
//
//        // Bind geometry pass textures.
//        Backend::Rendering::BindTextureWithSampler(localLightingShader, fbo_.GetNamedRenderTarget("position"), 0);
//        Backend::Rendering::BindTextureWithSampler(localLightingShader, fbo_.GetNamedRenderTarget("normal"), 1);
//        Backend::Rendering::BindTextureWithSampler(localLightingShader, fbo_.GetNamedRenderTarget("ambient"), 2);
//        Backend::Rendering::BindTextureWithSampler(localLightingShader, fbo_.GetNamedRenderTarget("diffuse"), 3);
//        Backend::Rendering::BindTextureWithSampler(localLightingShader, fbo_.GetNamedRenderTarget("specular"), 4);
//
//        ECS::Instance().IterateOver<Transform, Mesh, LocalLight>([localLightingShader](Transform& transform, Mesh& mesh, LocalLight& light) {
//            localLightingShader->SetUniform("modelTransform", transform.GetMatrix());
//
//            localLightingShader->SetUniform("lightPosition", transform.GetPosition());
//            localLightingShader->SetUniform("lightRadius", transform.GetScale().x);
//            localLightingShader->SetUniform("lightColor", light.color_);
//            localLightingShader->SetUniform("lightBrightness", light.brightness_);
//
//            mesh.Bind();
//            mesh.Render();
//            mesh.Unbind();
//        });
//
//        localLightingShader->Unbind();
    }

    glm::mat4 SceneCS562Project3::CalculateShadowMatrix() {
        // Construct shadow map transformation matrices.
        glm::mat4 projection = camera_.GetPerspectiveTransform();

        static glm::vec3 lightPosition = glm::vec3(3.0f, 8.0f, 3.0f);
        static glm::mat4 rotation = glm::rotate(glm::radians(0.5f * Time::Instance().dt), glm::vec3(0.0f, 1.0f, 0.0f));

        lightPosition = rotation * glm::vec4(lightPosition, 1.0f);
        glm::vec3 targetPosition = glm::vec3(0.0f);

        directionalLight_.direction_ = glm::normalize(targetPosition - lightPosition);

        glm::mat4 view = glm::lookAt(glm::normalize(lightPosition) * 12.0f, directionalLight_.direction_, glm::vec3(0.0f, 1.0f, 0.0f));

        return projection * view;
    }

    void SceneCS562Project3::InitializeBlurKernel() {
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
    void SceneCS562Project3::GenerateShadowMap() {
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

            ECS::Instance().IterateOver<Transform, Mesh>([shadowShader](Transform& transform, Mesh& mesh) {
                shadowShader->SetUniform("modelTransform", transform.GetMatrix());

                mesh.Bind();
                mesh.Render();
                mesh.Unbind();
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

    void SceneCS562Project3::BlurShadowMap() {
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

    void SceneCS562Project3::GenerateRandomPoints() {
        // Initialize uniform block layout.
        const int numRandomPoints = 40;

        {
            std::vector<UniformBufferElement> layoutElements;
            layoutElements.reserve(numRandomPoints + 1);

            layoutElements.emplace_back(UniformBufferElement { ShaderDataType::INT, "count" } );
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
            int kk;

            for (int k = 0; k < numRandomPoints; ++k) {
                kk = k;
                float u = 0.0f;
                for (float p = 0.5f; kk; p *= 0.5f, kk >>= 1) {
                    if (kk & 1) {
                        u += p;
                    }
                }

                points.emplace_back(u, (static_cast<float>(k) + 0.5f) / static_cast<float>(numRandomPoints)); // (u, v)
            }

            // Set data.
            const std::vector<UniformBufferElement>& layoutElements = randomPoints_.GetUniformBlock().GetUniformBlockLayout().GetBufferElements();
            int index = 0;

            randomPoints_.Bind();
            randomPoints_.SetSubData(layoutElements[index].GetBufferOffset(), 4, static_cast<const void*>(&numRandomPoints));
            for (const glm::vec2& value : points) {
                randomPoints_.SetSubData(layoutElements[index++].GetBufferOffset(), 8, static_cast<const void*>(&value));
            }
            randomPoints_.Unbind();
        }

    }

}