
#include "scenes/cs562/project2/project2.h"
#include "common/api/window.h"
#include "common/geometry/object_loader.h"
#include "common/utility/log.h"
#include "common/geometry/transform.h"
#include "common/ecs/ecs.h"
#include "common/application/time.h"

namespace Sandbox {

    SceneCS562Project2::SceneCS562Project2() : fbo_(2560, 1440),
                                               camera_(Window::Instance().GetWidth(), Window::Instance().GetHeight())
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

        camera_.SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    }

    void SceneCS562Project2::OnUpdate() {
        IScene::OnUpdate();
        camera_.Update();
    }

    void SceneCS562Project2::OnPreRender() {
        IScene::OnPreRender();
    }

    void SceneCS562Project2::OnRender() {
        IScene::OnRender();

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

        RenderDepthBuffer();

        Backend::Core::EnableFlag(GL_DEPTH_TEST);

        // Restore viewport.
        fbo_.Unbind();

        Window& window = Window::Instance();
        Backend::Core::SetViewport(0, 0, window.GetWidth(), window.GetHeight());
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
                fbo_.SaveRenderTargetsToDirectory("data/scenes/cs562_project_1/");
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
            ImGui::Image(reinterpret_cast<ImTextureID>(fbo_.GetNamedRenderTarget("depthTexture")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();
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
        shaderLibrary.CreateShader("Global Lighting Pass", { "assets/shaders/fsq.vert", "assets/shaders/global_lighting.frag" });
        shaderLibrary.CreateShader("Local Lighting Pass", { "assets/shaders/model.vert", "assets/shaders/local_lighting.frag" });
        shaderLibrary.CreateShader("Shadow Pass", { "assets/shaders/shadow.vert", "assets/shaders/shadow.frag" });
        shaderLibrary.CreateShader("Depth", { "assets/shaders/depth.vert", "assets/shaders/depth.frag" });
        shaderLibrary.CreateShader("FSQ", { "assets/shaders/fsq.vert", "assets/shaders/fsq.frag" });
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
            transform.SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
            transform.SetScale(glm::vec3(50.0f));
            transform.SetRotation(glm::vec3(270.0f, 0.0f, 0.0f));
        });
    }

    void SceneCS562Project2::ConfigureLights() {
        directionalLight_.brightness_ = 0.0f;

        ECS& ecs = ECS::Instance();
        Mesh mesh = OBJLoader::Instance().LoadFromFile(OBJLoader::Request("assets/models/sphere.obj"));

        int ID = ecs.CreateEntity("light");
        ecs.AddComponent<Mesh>(ID, mesh).Configure([](Mesh& mesh) {
            mesh.Complete();
        });

        ecs.GetComponent<Transform>(ID).Configure([](Transform& transform) {
            transform.SetPosition(glm::vec3(10.0f, 10.0f, 5.0f));
            transform.SetScale(glm::vec3(20.f));
        });
        ecs.AddComponent<LocalLight>(ID, glm::vec3(1.0f), 4.0f);
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

        // Depth texture.
        Texture* depthTexture = new Texture("depthTexture");
        depthTexture->Bind();
        depthTexture->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        depthTexture->Unbind();
        fbo_.AttachRenderTarget(depthTexture);

        // Depth buffer.
        Texture* depthBuffer = new Texture("depthBuffer");
        depthBuffer->Bind();
        depthBuffer->ReserveData(Texture::AttachmentType::DEPTH, contentWidth, contentHeight);
        depthBuffer->Unbind();
        fbo_.AttachRenderTarget(depthBuffer);

//        // Depth buffer (RBO).
//        RenderBufferObject* depthBuffer = new RenderBufferObject();
//        depthBuffer->Bind();
//        depthBuffer->ReserveData(contentWidth, contentHeight);
//        fbo_.AttachDepthBuffer(depthBuffer);

        // Attachment for rendering from the POV of the light.
        Texture* shadowTexture = new Texture("shadow");
        shadowTexture->Bind();
        shadowTexture->ReserveData(Texture::AttachmentType::COLOR, contentWidth, contentHeight);
        shadowTexture->Unbind();
        fbo_.AttachRenderTarget(shadowTexture);

        if (!fbo_.CheckStatus()) {
            throw std::runtime_error("Custom FBO is not complete.");
        }

        fbo_.Unbind();
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

        Shader* globalLightingShader = ShaderLibrary::Instance().GetShader("Global Lighting Pass");
        globalLightingShader->Bind();

        // Set camera uniforms.
        globalLightingShader->SetUniform("cameraPosition", camera_.GetPosition());

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

    void SceneCS562Project2::ShadowPass() {
        // Render to 'shadow' texture.
        fbo_.DrawBuffers(7, 1);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Cache camera information.
        const glm::vec3& cameraPosition = camera_.GetPosition();
        const glm::vec3& cameraLookAt = camera_.GetForwardVector();
        const glm::vec3& cameraUp = camera_.GetUpVector();

        Shader* shadowShader = ShaderLibrary::Instance().GetShader("Shadow Pass");
        shadowShader->Bind();

        // Position camera at the light looking at the origin.


        // Render all geometry.
        ECS::Instance().IterateOver<Transform, Mesh>([shadowShader](Transform& transform, Mesh& mesh) {
            mesh.Bind();
            mesh.Render();
            mesh.Unbind();
        });
    }

    void SceneCS562Project2::RenderDepthBuffer() {
        // Render to depth texturing using FSQ.
        fbo_.DrawBuffers(6, 1);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT);

        Shader* depthShader = ShaderLibrary::Instance().GetShader("Depth");
        depthShader->Bind();

        // Uniforms.
        depthShader->SetUniform("cameraNearPlane", 0.1f);
        depthShader->SetUniform("cameraFarPlane", 25.0f);
        Backend::Rendering::BindTextureWithSampler(depthShader, fbo_.GetNamedRenderTarget("depthBuffer"), "inputTexture", 0);

        // Render to depth texture using FSQ.
        Backend::Rendering::DrawFSQ();

        depthShader->Unbind();
    }

}