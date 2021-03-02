
#include <scenes/deferred_rendering.h>
#include <framework/imgui_log.h>
#include <framework/backend.h>

namespace Sandbox {

    SceneDeferredRendering::SceneDeferredRendering(int width, int height) : Scene("Deferred Rendering", width, height),
                                                                            _fbo(2560, 1440) {

    }

    SceneDeferredRendering::~SceneDeferredRendering() {

    }

    void SceneDeferredRendering::OnInit() {
        InitializeShaders();
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

        Shader* singleColorShader = shaderLibrary.GetShader("SingleColor");
        singleColorShader->Bind();
        singleColorShader->SetUniform("cameraTransform", _camera.GetMatrix());

        for (Model* model : _modelManager.GetModels()) {
            Transform& transform = model->GetTransform();
            Mesh& mesh = model->GetMesh();
            Material* material = model->GetMaterial(singleColorShader);

            // Pre render stage.
            if (material) {
                const glm::mat4& modelTransform = transform.GetMatrix();
                singleColorShader->SetUniform("modelTransform", modelTransform);
                singleColorShader->SetUniform("normalTransform", glm::transpose(glm::inverse(modelTransform)));

                // Bind all related uniforms with this shader.
                material->Bind(singleColorShader);
            }

            // Render stage.
            mesh.Bind();
            Backend::Rendering::DrawIndexed(mesh.GetVAO(), mesh.GetRenderingPrimitive());
            mesh.Unbind();

            // Post render stage.
        }

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
        if (ImGui::Begin("Overview", nullptr)) {
            ImGui::Text("Render time:");
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        ImGui::End();

        // Draw the final output from the hybrid rendering pipeline.
        if (ImGui::Begin("Framebuffer", nullptr, ImGuiWindowFlags_NoScrollbar)) {
            // Ensure proper scene image scaling.
            float maxWidth = ImGui::GetWindowContentRegionWidth();
            float aspect = static_cast<float>(_window.GetWidth()) / static_cast<float>(_window.GetHeight());
            ImVec2 imageSize = ImVec2(maxWidth, maxWidth / aspect);
            ImGui::SetCursorPosY(ImGui::GetItemRectSize().y + (ImGui::GetWindowSize().y - ImGui::GetItemRectSize().y - imageSize.y) * 0.5f);

            ImGui::Image(reinterpret_cast<ImTextureID>(_fbo.GetNamedRenderTarget("output")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
        }

        ImGui::End();

        log.OnImGui();
    }

    void SceneDeferredRendering::OnShutdown() {

    }

    void SceneDeferredRendering::InitializeShaders() {
        ShaderLibrary& shaderLibrary = ShaderLibrary::GetInstance();

        shaderLibrary.AddShader("SingleColor", { "assets/shaders/color.vert", "assets/shaders/color.frag" });
    }

    void SceneDeferredRendering::InitializeMaterials() {
        ShaderLibrary& shaderLibrary = ShaderLibrary::GetInstance();
        MaterialLibrary& materialLibrary = MaterialLibrary::GetInstance();

        materialLibrary.AddMaterial("SingleColor", shaderLibrary.GetShader("SingleColor"), {
            { "surfaceColor", glm::vec3(1.0f) }
        });
    }

    void SceneDeferredRendering::ConfigureModels() {
        MaterialLibrary& materialLibrary = MaterialLibrary::GetInstance();

        Model* vikingRoom = _modelManager.AddModelFromFile("viking room", "assets/models/viking_room.obj");
        vikingRoom->AddMaterial(materialLibrary.GetMaterialInstance("SingleColor"));
    }

    void SceneDeferredRendering::ConstructFBO() {
        _fbo.BindForReadWrite();

        // Output texture.
        Texture* outputTexture = new Texture("output");
        outputTexture->Bind();
        outputTexture->ReserveData(Texture::AttachmentType::COLOR, 2560, 1440);
        _fbo.AttachRenderTarget(outputTexture);

        _fbo.DrawBuffers();

        // Depth buffer (RBO).
        RenderBufferObject* depthBuffer = new RenderBufferObject();
        depthBuffer->Bind();
        depthBuffer->ReserveData(2560, 1440);
        _fbo.AttachDepthBuffer(depthBuffer);

        if (!_fbo.CheckStatus()) {
            throw std::runtime_error("Custom FBO is not complete.");
        }

        _fbo.Unbind();
    }

}
