
#include "scenes/cs562/project2/project2.h"
#include "common/api/window.h"
#include "common/geometry/object_loader.h"
#include "common/utility/log.h"
#include "common/geometry/transform.h"
#include "common/ecs/ecs.h"
#include "common/application/time.h"

namespace Sandbox {

    SceneCS562Project2::SceneCS562Project2() : fbo_(2560, 1440),
                                               shadowMap_(2048, 2048),
                                               camera_(Window::Instance().GetWidth(), Window::Instance().GetHeight()),
                                               blurKernelRadius_(25)
                                               {
    }

    SceneCS562Project2::~SceneCS562Project2() {
    }

    void SceneCS562Project2::OnInit() {
        IScene::OnInit();

        InitializeShaders();
        InitializeMaterials();

        ConfigureLights();
        ConfigureModels();
        ConstructFBO();
        ConstructShadowMap();
        InitializeBlurKernel();

        camera_.SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    }

    void SceneCS562Project2::OnUpdate() {
        IScene::OnUpdate();
        camera_.Update();

        // Rotate center model.
        ComponentWrapper<Transform> transform = ECS::Instance().GetComponent<Transform>("Bunny");
        transform->SetRotation(transform->GetRotation() + glm::vec3(0.0f, 10.0f, 0.0f) * Time::Instance().dt);
    }

    void SceneCS562Project2::OnPreRender() {
        IScene::OnPreRender();
    }

    void SceneCS562Project2::OnRender() {
        IScene::OnRender();

        Backend::Core::EnableFlag(GL_DEPTH_TEST);

        // Render shadow map.
        glm::vec4 viewport = Backend::Core::GetViewport();

        Backend::Core::SetViewport(0, 0, shadowMap_.GetWidth(), shadowMap_.GetHeight());
        shadowMap_.BindForReadWrite();

        // Clear all buffers.
        shadowMap_.DrawBuffers();
        Backend::Core::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    void SceneCS562Project2::OnPostRender() {
        IScene::OnPostRender();
    }

    void SceneCS562Project2::OnImGui() {
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

    void SceneCS562Project2::OnShutdown() {
        IScene::OnShutdown();
    }

    void SceneCS562Project2::OnWindowResize(int width, int height) {
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

    void SceneCS562Project2::InitializeShaders() {
        ShaderLibrary& shaderLibrary = ShaderLibrary::Instance();

        shaderLibrary.CreateShader("Geometry Pass", { "assets/shaders/geometry_buffer.vert", "assets/shaders/geometry_buffer.frag" });
        shaderLibrary.CreateShader("Global Lighting Shadow Pass", { "assets/shaders/fsq.vert", "assets/shaders/global_lighting_shadow.frag" });
        shaderLibrary.CreateShader("Local Lighting Pass", { "assets/shaders/model.vert", "assets/shaders/local_lighting.frag" });
        shaderLibrary.CreateShader("Shadow Map Pass", { "assets/shaders/shadow.vert", "assets/shaders/shadow.frag" });
        shaderLibrary.CreateShader("Shadow Out Pass", { "assets/shaders/fsq.vert", "assets/shaders/shadow_out.frag" });
        shaderLibrary.CreateShader("FSQ", { "assets/shaders/fsq.vert", "assets/shaders/fsq.frag" });
        shaderLibrary.CreateShader("Blur Horizontal", { "assets/shaders/blur_horizontal.comp" });
        shaderLibrary.CreateShader("Blur Vertical", { "assets/shaders/blur_vertical.comp" });
    }

    void SceneCS562Project2::InitializeMaterials() {
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

    void SceneCS562Project2::ConfigureModels() {
        ECS& ecs = ECS::Instance();

        // Bunny.
        int bunny = ecs.CreateEntity("Bunny");

        ecs.AddComponent<Mesh>(bunny, OBJLoader::Instance().LoadFromFile(OBJLoader::Request("assets/models/bunny_high_poly.obj"))).Configure([](Mesh& mesh) {
            mesh.Complete();
        });

        ecs.AddComponent<MaterialCollection>(bunny).Configure([this](MaterialCollection& materialCollection) {
            Material* phong = materialLibrary_.GetMaterialInstance("Phong");
            phong->GetUniform("ambientCoefficient")->SetData(glm::vec3(0.05f));

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
            phong->GetUniform("specularCoefficient")->SetData(glm::vec3(0.85f));

            materialCollection.SetMaterial(phong);
        });

        ecs.GetComponent<Transform>(floor).Configure([](Transform& transform) {
            transform.SetPosition(glm::vec3(0.0f, -4.0f, 0.0f));
            transform.SetScale(glm::vec3(20.0f));
            transform.SetRotation(glm::vec3(270.0f, 0.0f, 0.0f));
        });
    }

    void SceneCS562Project2::ConfigureLights() {
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

    void SceneCS562Project2::ConstructFBO() {
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

        // Output texture.
        Texture* outputTexture = new Texture("output");
        outputTexture->Bind();
        outputTexture->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        outputTexture->Unbind();
        fbo_.AttachRenderTarget(outputTexture);

        // Depth buffer.
//        Texture* depthBuffer = new Texture("depthBuffer");
//        depthBuffer->Bind();
//        depthBuffer->ReserveData(Texture::AttachmentType::DEPTH, contentWidth, contentHeight);
//        depthBuffer->Unbind();
//        fbo_.AttachRenderTarget(depthBuffer);

        // Depth buffer (RBO).
        RenderBufferObject* depthBuffer = new RenderBufferObject();
        depthBuffer->Bind();
        depthBuffer->ReserveData(contentWidth, contentHeight);
        fbo_.AttachDepthBuffer(depthBuffer);

        if (!fbo_.CheckStatus()) {
            throw std::runtime_error("Custom FBO is not complete.");
        }

        fbo_.Unbind();
    }

    void SceneCS562Project2::ConstructShadowMap() {
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

    void SceneCS562Project2::GeometryPass() {
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
    }

    void SceneCS562Project2::GlobalLightingPass() {
        // Render to 'output' texture.
        fbo_.DrawBuffers(5, 1);

        Backend::Core::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT); // We are not touching depth buffer here.

        Shader* globalLightingShader = ShaderLibrary::Instance().GetShader("Global Lighting Shadow Pass");
        globalLightingShader->Bind();

        // Set camera uniforms.
        globalLightingShader->SetUniform("cameraPosition", camera_.GetPosition());
        globalLightingShader->SetUniform("shadowTransform", CalculateShadowMatrix());

        // Set global lighting uniforms.
        globalLightingShader->SetUniform("lightDirection", directionalLight_.direction_);
        globalLightingShader->SetUniform("lightColor", directionalLight_.color_);
        globalLightingShader->SetUniform("lightBrightness", directionalLight_.brightness_);

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

    void SceneCS562Project2::LocalLightingPass() {
        // Render to 'output' texture.
        fbo_.DrawBuffers(5, 1);
        // No clearing here.

        Shader* localLightingShader = ShaderLibrary::Instance().GetShader("Local Lighting Pass");
        localLightingShader->Bind();

        localLightingShader->SetUniform("resolution", glm::vec2(fbo_.GetWidth(), fbo_.GetHeight()));

        // Set camera uniforms.
        localLightingShader->SetUniform("cameraPosition", camera_.GetPosition());
        localLightingShader->SetUniform("cameraTransform", camera_.GetCameraTransform());

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

    glm::mat4 SceneCS562Project2::CalculateShadowMatrix() {
        // Construct shadow map transformation matrices.
        float distance = 15.0f;
        glm::mat4 projection = glm::ortho(-distance, distance, -distance, distance, camera_.GetNearPlaneDistance(), camera_.GetFarPlaneDistance());
        glm::mat4 view = glm::lookAt(-directionalLight_.direction_, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));

        return projection * view;
    }

    void SceneCS562Project2::InitializeBlurKernel() {
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
            float p = (static_cast<float>(i) / s) * (static_cast<float>(i) / s);
            kernel[offset++] = glm::exp(-0.5f * p);
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

    void SceneCS562Project2::GenerateShadowMap() {
        {
            // Render four channel depth buffer.
            shadowMap_.DrawBuffers(1, 1);
            Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT);

            Shader* shadowShader = ShaderLibrary::Instance().GetShader("Shadow Map Pass");
            shadowShader->Bind();
            shadowShader->SetUniform("shadowTransform", CalculateShadowMatrix());

            ECS::Instance().IterateOver<Transform, Mesh>([shadowShader](Transform& transform, Mesh& mesh) {
                shadowShader->SetUniform("modelTransform", transform.GetMatrix());

                mesh.Bind();
                mesh.Render();
                mesh.Unbind();
            });

            shadowShader->Unbind();
        }

        Backend::Core::DisableFlag(GL_DEPTH_TEST);

        {
            // Render single channel shadow map out for debug viewing.
            shadowMap_.DrawBuffers(0, 1);
            Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT);

            Shader* shadowShader = ShaderLibrary::Instance().GetShader("Shadow Out Pass");
            shadowShader->Bind();
            shadowShader->SetUniform("cameraNearPlane", camera_.GetNearPlaneDistance());
            shadowShader->SetUniform("cameraFarPlane", camera_.GetFarPlaneDistance());
            Backend::Rendering::BindTextureWithSampler(shadowShader, shadowMap_.GetNamedRenderTarget("depth"), "depthTexture", 0);

            Backend::Rendering::DrawFSQ();

            shadowShader->Unbind();
        }

        Backend::Core::EnableFlag(GL_DEPTH_TEST);
    }

    void SceneCS562Project2::BlurShadowMap() {
        shadowMap_.DrawBuffers(3, 2);
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

        shadowMap_.DrawBuffers(2, 1);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT);

        Shader* depthShader = ShaderLibrary::Instance().GetShader("Shadow Out Pass");
        depthShader->Bind();
        depthShader->SetUniform("cameraNearPlane", camera_.GetNearPlaneDistance());
        depthShader->SetUniform("cameraFarPlane", camera_.GetFarPlaneDistance());

        if (blurKernelRadius_ > 0) {
            Backend::Rendering::BindTextureWithSampler(depthShader, shadowMap_.GetNamedRenderTarget("blur vertical"), "depthTexture", 0);
        }
        else {
            Backend::Rendering::BindTextureWithSampler(depthShader, shadowMap_.GetNamedRenderTarget("depth"), "depthTexture", 0);
        }

        Backend::Rendering::DrawFSQ();

        depthShader->Unbind();

        Backend::Core::EnableFlag(GL_DEPTH_TEST);
    }

}