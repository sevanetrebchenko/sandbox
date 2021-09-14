
#include <scenes/ascii_post_processing/ascii_post_processing.h>
#include <framework/primitive_loader.h>
#include <framework/shader_library.h>
#include <framework/texture_library.h>
#include <framework/material_library.h>
#include <framework/imgui_log.h>
#include <framework/scene_data.h>

namespace Sandbox {

    SceneAsciiPostProcessing::SceneAsciiPostProcessing() : IScene(),
                                                           _fbo(2560, 1440),
                                                           _characterMap("data/scenes/ascii_post_processing/fontsheets/ascii5x5.txt") {
    }

    SceneAsciiPostProcessing::~SceneAsciiPostProcessing() {

    }

    void SceneAsciiPostProcessing::OnInit() {
        InitializeShaders();
        InitializeTextures();
        InitializeMaterials();

        ConstructFBO();
        ConfigureModels();

        ConstructAsciiMaps();
    }

    void SceneAsciiPostProcessing::OnUpdate(float dt) {
        _characterMap.UpdateData();
    }

    void SceneAsciiPostProcessing::OnPreRender() {
        Backend::Core::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SceneAsciiPostProcessing::OnRender() {
        ShaderLibrary* shaderLibrary = Singleton<ShaderLibrary>::GetInstance();
        ModelManager* modelManager = Singleton<ModelManager>::GetInstance();
        Window* window = Singleton<Window>::GetInstance();

        _fbo.BindForReadWrite();
        _fbo.DrawBuffers(0, 1);
        Backend::Core::SetViewport(0, 0, _fbo.GetWidth(), _fbo.GetHeight()); // Set viewport.
        Backend::Core::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Shader* textureShader = shaderLibrary->GetShader("Texture");
        textureShader->Bind();
        textureShader->SetUniform("cameraTransform", camera_.GetMatrix());

        for (Model* model : modelManager->GetModels()) {
            Transform& transform = model->GetTransform();
            Mesh& mesh = model->GetMesh();
            Material* material = model->GetMaterial("Texture");

            // Pre render stage.
            if (material) {
                const glm::mat4& modelTransform = transform.GetMatrix();
                textureShader->SetUniform("modelTransform", modelTransform);
                textureShader->SetUniform("normalTransform", glm::transpose(glm::inverse(modelTransform)));

                // Bind all related uniforms with this shader.
                material->Bind(textureShader);
            }

            // Render stage.
            mesh.Bind();
            Backend::Rendering::DrawIndexed(mesh.GetVAO(), mesh.GetRenderingPrimitive());
            mesh.Unbind();

            // Post render stage.
        }
        textureShader->Unbind();

        _fbo.DrawBuffers(1, 1);
        Shader* asciiShader = shaderLibrary->GetShader("Ascii");

        asciiShader->Bind();
        asciiShader->SetUniform("cameraTransform", camera_.GetMatrix());
        Backend::Rendering::BindTextureWithSampler(asciiShader, _fbo.GetNamedRenderTarget("regularOutput"), "inputTexture", 0);
        asciiShader->SetUniform("resolution", glm::vec2(_fbo.GetWidth(), _fbo.GetHeight()));
        Backend::Rendering::DrawFSQ();
        asciiShader->Unbind();

        _fbo.Unbind();

        Backend::Core::SetViewport(0, 0, window->GetWidth(), window->GetHeight()); // Restore viewport.
    }

    void SceneAsciiPostProcessing::OnPostRender() {

    }

    void SceneAsciiPostProcessing::OnImGui() {
        Window* window = Singleton<Window>::GetInstance();

        // Framework overview.
        if (ImGui::Begin("Options", nullptr)) {
            if (ImGui::Button("Take Screenshot")) {
                _fbo.SaveRenderTargetsToDirectory(sceneData_->dataDirectory_);
            }
        }
        ImGui::End();

        // Draw the scene framebuffer without any post processing.
        if (ImGui::Begin("Scene Framebuffer")) {
            // Ensure proper scene image scaling.
            float maxWidth = ImGui::GetWindowContentRegionWidth();
            float aspect = static_cast<float>(window->GetWidth()) / static_cast<float>(window->GetHeight());
            ImVec2 imageSize = ImVec2(maxWidth, maxWidth / aspect);

            ImGui::Image(reinterpret_cast<ImTextureID>(_fbo.GetNamedRenderTarget("regularOutput")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();

        // Draw the scene framebuffer with post processing.
        if (ImGui::Begin("Post-Processing")) {
            // Ensure proper scene image scaling.
            float maxWidth = ImGui::GetWindowContentRegionWidth();
            float aspect = static_cast<float>(window->GetWidth()) / static_cast<float>(window->GetHeight());
            ImVec2 imageSize = ImVec2(maxWidth, maxWidth / aspect);

            ImGui::Image(reinterpret_cast<ImTextureID>(_fbo.GetNamedRenderTarget("asciiOutput")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();
    }

    void SceneAsciiPostProcessing::OnShutdown() {

    }

    void SceneAsciiPostProcessing::InitializeShaders() {
        ShaderLibrary* shaderLibrary = Singleton<ShaderLibrary>::GetInstance();

        shaderLibrary->AddShader("SingleColor", { "assets/shaders/color.vert", "assets/shaders/color.frag" });
        shaderLibrary->AddShader("Texture", { "assets/shaders/texture.vert", "assets/shaders/texture.frag" });
        shaderLibrary->AddShader("Ascii", { "assets/shaders/ascii.vert", "assets/shaders/ascii.frag" });
    }

    void SceneAsciiPostProcessing::InitializeTextures() {
        TextureLibrary* textureLibrary = Singleton<TextureLibrary>::GetInstance();

        textureLibrary->AddTexture("viking room", "assets/textures/viking_room.png");
    }

    void SceneAsciiPostProcessing::InitializeMaterials() {
        MaterialLibrary* materialLibrary = Singleton<MaterialLibrary>::GetInstance();
        TextureLibrary* textureLibrary = Singleton<TextureLibrary>::GetInstance();

        // Single color material.
        Material* singleColorMaterial = new Material("SingleColor", {
                { "surfaceColor", glm::vec3(1.0f) }
        });
        singleColorMaterial->GetUniform("surfaceColor")->UseColorPicker(true);
        materialLibrary->AddMaterial(singleColorMaterial);

        // Textured material.
        Material* textureMaterial = new Material("Texture", {
                { "modelTexture", TextureSampler(textureLibrary->GetTexture("viking room"), 0) }
        });
        materialLibrary->AddMaterial(textureMaterial);
    }

    void SceneAsciiPostProcessing::ConfigureModels() {
        MaterialLibrary* materialLibrary = Singleton<MaterialLibrary>::GetInstance();
        ModelManager* modelManager = Singleton<ModelManager>::GetInstance();

        Model* vikingRoom = modelManager->AddModelFromFile("viking room", "assets/models/viking_room.obj");
        vikingRoom->AddMaterial(materialLibrary->GetMaterialInstance("SingleColor"));
        vikingRoom->AddMaterial(materialLibrary->GetMaterialInstance("Texture"));

        Transform& vikingRoomTransform = vikingRoom->GetTransform();
        vikingRoomTransform.SetRotation(glm::vec3(-90.0f, 0.0f, -135.0f));
        vikingRoomTransform.SetScale(glm::vec3(3.5f, 3.5f, 3.5f));
    }

    void SceneAsciiPostProcessing::ConstructFBO() {
        _fbo.BindForReadWrite();

        // Output texture.
        Texture* regularOutputTexture = new Texture("regularOutput");
        regularOutputTexture->Bind();
        regularOutputTexture->ReserveData(Texture::AttachmentType::COLOR, 2560, 1440);
        regularOutputTexture->Unbind();
        _fbo.AttachRenderTarget(regularOutputTexture);

        Texture* asciiOutputTexture = new Texture("asciiOutput");
        asciiOutputTexture->Bind();
        asciiOutputTexture->ReserveData(Texture::AttachmentType::COLOR, 2560, 1440);
        asciiOutputTexture->Unbind();
        _fbo.AttachRenderTarget(asciiOutputTexture);

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

    void SceneAsciiPostProcessing::ConstructAsciiMaps() {
        _characterMap.UpdateData();
    }

}