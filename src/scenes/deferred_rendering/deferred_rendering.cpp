
#include <scenes/deferred_rendering/deferred_rendering.h>
#include <framework/imgui_log.h>
#include <framework/backend.h>
#include <framework/material_library.h>
#include <framework/shader_library.h>
#include <framework/texture_library.h>
#include <framework/primitive_loader.h>
#include <framework/imgui_log.h>

namespace Sandbox {

    SceneDeferredRendering::SceneDeferredRendering(int width, int height) : Scene("Deferred Rendering", width, height),
                                                                            _fbo(2560, 1440) {
        _dataDirectory = "data/scenes/deferred_rendering";
    }

    SceneDeferredRendering::~SceneDeferredRendering() {

    }

    void SceneDeferredRendering::OnInit() {
        InitializeShaders();
        InitializeTextures();
        InitializeMaterials();

        ConstructFBO();
        ConfigureModels();
    }

    void SceneDeferredRendering::OnUpdate(float dt) {
        ShaderLibrary& shaderLibrary = ShaderLibrary::GetInstance();
        ImGuiLog& log = ImGuiLog::GetInstance();

        // Recompile shaders.
        try {
            shaderLibrary.RecompileAllModified();
        }
        catch (std::runtime_error& err) {
            log.LogError("Shader recompilation failed: %s", err.what());
        }

        _modelManager.Update(dt);
    }

    void SceneDeferredRendering::OnPreRender() {
        Backend::Core::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SceneDeferredRendering::OnRender() {
        ShaderLibrary& shaderLibrary = ShaderLibrary::GetInstance();

        _fbo.BindForReadWrite();

        Backend::Core::SetViewport(0, 0, _fbo.GetWidth(), _fbo.GetHeight()); // Set viewport.
        Backend::Core::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Shader* textureShader = shaderLibrary.GetShader("Texture");
        RenderWithShader(textureShader);
        textureShader->Unbind();

        _fbo.Unbind();

        Backend::Core::SetViewport(0, 0, _window.GetWidth(), _window.GetHeight()); // Restore viewport.
    }

    void SceneDeferredRendering::OnPostRender() {

    }

    void SceneDeferredRendering::OnImGui() {
        ShaderLibrary& shaderLibrary = ShaderLibrary::GetInstance();
        MaterialLibrary& materialLibrary = MaterialLibrary::GetInstance();
        ImGuiLog& log = ImGuiLog::GetInstance();

        // Enable window docking.
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // Framework overview.
        if (ImGui::Begin("Overview")) {
            ImGui::Text("Render time:");
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        ImGui::End();

        // Draw the final output from the hybrid rendering pipeline.
        if (ImGui::Begin("Framebuffer")) {
            // Ensure proper scene image scaling.
            float maxWidth = ImGui::GetWindowContentRegionWidth();
            float aspect = static_cast<float>(_window.GetWidth()) / static_cast<float>(_window.GetHeight());
            ImVec2 imageSize = ImVec2(maxWidth, maxWidth / aspect);
            ImGui::SetCursorPosY(ImGui::GetItemRectSize().y + (ImGui::GetWindowSize().y - ImGui::GetItemRectSize().y - imageSize.y) * 0.5f);

            ImGui::Image(reinterpret_cast<ImTextureID>(_fbo.GetNamedRenderTarget("output")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
        }

        ImGui::End();

        // Draw individual deferred rendering textures.
        if (ImGui::Begin("Debug Textures")) {
            // Ensure proper scene image scaling.
            float maxWidth = ImGui::GetWindowContentRegionWidth();
            float aspect = static_cast<float>(_window.GetWidth()) / static_cast<float>(_window.GetHeight());
            ImVec2 imageSize = ImVec2(maxWidth, maxWidth / aspect);
            ImGui::SetCursorPosY(ImGui::GetItemRectSize().y + (ImGui::GetWindowSize().y - ImGui::GetItemRectSize().y - imageSize.y) * 0.5f);

            ImGui::Image(reinterpret_cast<ImTextureID>(_fbo.GetNamedRenderTarget("position")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();
            ImGui::Image(reinterpret_cast<ImTextureID>(_fbo.GetNamedRenderTarget("depth")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();

        // ImGui log output.
        log.OnImGui();

        // Materials.
        materialLibrary.OnImGui();

        _modelManager.OnImGui();
    }

    void SceneDeferredRendering::OnShutdown() {

    }

    void SceneDeferredRendering::InitializeShaders() {
        ShaderLibrary& shaderLibrary = ShaderLibrary::GetInstance();

        shaderLibrary.AddShader("SingleColor", { "assets/shaders/color.vert", "assets/shaders/color.frag" });
        shaderLibrary.AddShader("Texture", { "assets/shaders/texture.vert", "assets/shaders/texture.frag" });
//        shaderLibrary.AddShader("Depth", { "assets/shaders/depth.vert", "assets/shaders/depth.frag" });
//        shaderLibrary.AddShader("PhongShading", { "assets/shaders/phong_shading.vert", "assets/shaders/phong_shading.frag" });
    }

    void SceneDeferredRendering::InitializeTextures() {
        TextureLibrary& textureLibrary = TextureLibrary::GetInstance();

        textureLibrary.AddTexture("viking room", "assets/textures/viking_room.png");
    }

    void SceneDeferredRendering::InitializeMaterials() {
        ShaderLibrary& shaderLibrary = ShaderLibrary::GetInstance();
        MaterialLibrary& materialLibrary = MaterialLibrary::GetInstance();
        TextureLibrary& textureLibrary = TextureLibrary::GetInstance();

        // Single color material.
        Shader* singleColorShader = shaderLibrary.GetShader("SingleColor");
        Material* singleColorMaterial = new Material("SingleColor", singleColorShader, {
            { "surfaceColor", glm::vec3(1.0f) }
        });
        singleColorMaterial->GetUniform("surfaceColor")->UseColorPicker(true);
        materialLibrary.AddMaterial(singleColorMaterial);

        // Textured material.
        Shader* textureShader = shaderLibrary.GetShader("Texture");
        Material* textureMaterial = new Material("Texture", textureShader, {
            { "modelTexture", TextureSampler(textureLibrary.GetTexture("viking room"), 0) }
        });
        materialLibrary.AddMaterial(textureMaterial);

        // Phong shading material.
        Shader* phongShader = shaderLibrary.GetShader("PhongShading");
        Material* phongMaterial = new Material("PhongShading", phongShader, {
            { "ambientCoefficient", glm::vec3(0.5f) },
            { "diffuseCoefficient", glm::vec3(0.5f) },
            { "specularCoefficient", glm::vec3(1.0f) },
            { "specularExponent", 30.0f }
        });
        materialLibrary.AddMaterial(phongMaterial);
    }

    void SceneDeferredRendering::ConfigureModels() {
        MaterialLibrary& materialLibrary = MaterialLibrary::GetInstance();

        Model* bunny = _modelManager.AddModelFromFile("bunny", "assets/models/bunny_high_poly.obj");
        bunny->AddMaterial(materialLibrary.GetMaterialInstance("PhongShading"));

//        Model* vikingRoom = _modelManager.AddModelFromFile("viking room", "assets/models/viking_room.obj");
//        vikingRoom->AddMaterial(materialLibrary.GetMaterialInstance("SingleColor"));
//        vikingRoom->AddMaterial(materialLibrary.GetMaterialInstance("Texture"));
//
//        Transform& vikingRoomTransform = vikingRoom->GetTransform();
//        vikingRoomTransform.SetRotation(glm::vec3(-90.0f, 0.0f, -135.0f));
//        vikingRoomTransform.SetScale(glm::vec3(3.5f, 3.5f, 3.5f));
    }

    void SceneDeferredRendering::ConstructFBO() {
        _fbo.BindForReadWrite();

        // Position.
        Texture* positionTexture = new Texture("position");
        positionTexture->Bind();
        positionTexture->ReserveData(Texture::AttachmentType::COLOR, 2560, 1440);
        positionTexture->Unbind();
        _fbo.AttachRenderTarget(positionTexture);

        // Depth buffer.
        Texture* depthTexture = new Texture("depth");
        depthTexture->Bind();
        depthTexture->ReserveData(Texture::AttachmentType::DEPTH, 2560, 1440);
        depthTexture->Unbind();
        _fbo.AttachRenderTarget(depthTexture);

        // Output texture.
        Texture* outputTexture = new Texture("output");
        outputTexture->Bind();
        outputTexture->ReserveData(Texture::AttachmentType::COLOR, 2560, 1440);
        outputTexture->Unbind();
        _fbo.AttachRenderTarget(outputTexture);

        _fbo.DrawBuffers();

//        // Depth buffer (RBO).
//        RenderBufferObject* depthBuffer = new RenderBufferObject();
//        depthBuffer->Bind();
//        depthBuffer->ReserveData(2560, 1440);
//        _fbo.AttachDepthBuffer(depthBuffer);

        if (!_fbo.CheckStatus()) {
            throw std::runtime_error("Custom FBO is not complete.");
        }

        _fbo.Unbind();
    }

    void SceneDeferredRendering::RenderWithShader(Shader *shaderProgram) {
        shaderProgram->Bind();
        shaderProgram->SetUniform("cameraTransform", _camera.GetMatrix());

        for (Model* model : _modelManager.GetModels()) {
            Transform& transform = model->GetTransform();
            Mesh& mesh = model->GetMesh();
            Material* material = model->GetMaterial(shaderProgram);

            // Pre render stage.
            if (material) {
                const glm::mat4& modelTransform = transform.GetMatrix();
                shaderProgram->SetUniform("modelTransform", modelTransform);
                shaderProgram->SetUniform("normalTransform", glm::transpose(glm::inverse(modelTransform)));

                // Bind all related uniforms with this shader.
                material->Bind(shaderProgram);
            }

            // Render stage.
            mesh.Bind();
            Backend::Rendering::DrawIndexed(mesh.GetVAO(), mesh.GetRenderingPrimitive());
            mesh.Unbind();

            // Post render stage.
        }
    }

}
