
#include <scenes/ascii_post_processing/ascii_post_processing.h>
#include <scenes/ascii_post_processing/ascii_character_map_manager.h>
#include <framework/primitive_loader.h>
#include <framework/shader_library.h>
#include <framework/texture_library.h>
#include <framework/material_library.h>
#include <framework/imgui_log.h>

namespace Sandbox {

    SceneAsciiPostProcessing::SceneAsciiPostProcessing(int width, int height) : Scene("Ascii Post Processing", width, height),
                                                                                _fbo(2560, 1440),
                                                                                _fsq(PrimitiveLoader::GetInstance().LoadPrimitive(PrimitiveLoader::PrimitiveType::PLANE)),
                                                                                _characterMap(5, 5) {
        _dataDirectory = "data/scenes/ascii_post_processing/";
        _fsq.Complete();
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

        _characterMap.UpdateData(&_ubo);
    }

    void SceneAsciiPostProcessing::OnPreRender() {
        Backend::Core::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SceneAsciiPostProcessing::OnRender() {
        ShaderLibrary& shaderLibrary = ShaderLibrary::GetInstance();

        _fbo.BindForReadWrite();
        _fbo.DrawBuffers(0, 1);
        Backend::Core::SetViewport(0, 0, _fbo.GetWidth(), _fbo.GetHeight()); // Set viewport.
        Backend::Core::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Shader* textureShader = shaderLibrary.GetShader("Texture");
        RenderWithShader(textureShader);
        textureShader->Unbind();

        _fbo.DrawBuffers(1, 1);
        Shader* asciiShader = shaderLibrary.GetShader("Ascii");
        asciiShader->Bind();
        Backend::Rendering::BindTextureWithSampler(asciiShader, _fbo.GetNamedRenderTarget("regularOutput"), "inputTexture", 0);
        RenderFSQ();
        asciiShader->Unbind();

        _fbo.Unbind();

        Backend::Core::SetViewport(0, 0, _window.GetWidth(), _window.GetHeight()); // Restore viewport.
    }

    void SceneAsciiPostProcessing::OnPostRender() {

    }

    void SceneAsciiPostProcessing::OnImGui() {
        ShaderLibrary& shaderLibrary = ShaderLibrary::GetInstance();
        MaterialLibrary& materialLibrary = MaterialLibrary::GetInstance();
        ImGuiLog& log = ImGuiLog::GetInstance();

        // Enable window docking.
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // Framework overview.
        if (ImGui::Begin("Overview", nullptr)) {
            ImGui::Text("Render time:");
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::Separator();

            if (ImGui::Button("Take Screenshot")) {
                _fbo.SaveRenderTargetsToDirectory(_dataDirectory);
            }
        }
        ImGui::End();

        // Draw the scene framebuffer without any post processing.
        if (ImGui::Begin("Scene Framebuffer")) {
            // Ensure proper scene image scaling.
            float maxWidth = ImGui::GetWindowContentRegionWidth();
            float aspect = static_cast<float>(_window.GetWidth()) / static_cast<float>(_window.GetHeight());
            ImVec2 imageSize = ImVec2(maxWidth, maxWidth / aspect);

            ImGui::Image(reinterpret_cast<ImTextureID>(_fbo.GetNamedRenderTarget("regularOutput")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();

        // Draw the scene framebuffer with post processing.
        if (ImGui::Begin("Post-Processing")) {
            // Ensure proper scene image scaling.
            float maxWidth = ImGui::GetWindowContentRegionWidth();
            float aspect = static_cast<float>(_window.GetWidth()) / static_cast<float>(_window.GetHeight());
            ImVec2 imageSize = ImVec2(maxWidth, maxWidth / aspect);

            ImGui::Image(reinterpret_cast<ImTextureID>(_fbo.GetNamedRenderTarget("asciiOutput")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();

        // ImGui log output.
        log.OnImGui();

        // Materials.
        materialLibrary.OnImGui();

        // Models.
        _modelManager.OnImGui();
    }

    void SceneAsciiPostProcessing::OnShutdown() {

    }

    void SceneAsciiPostProcessing::InitializeShaders() {
        ShaderLibrary& shaderLibrary = ShaderLibrary::GetInstance();

        shaderLibrary.AddShader("SingleColor", { "assets/shaders/color.vert", "assets/shaders/color.frag" });
        shaderLibrary.AddShader("Texture", { "assets/shaders/texture.vert", "assets/shaders/texture.frag" });
        shaderLibrary.AddShader("Ascii", { "assets/shaders/ascii.vert", "assets/shaders/ascii.frag" });
    }

    void SceneAsciiPostProcessing::InitializeTextures() {
        TextureLibrary& textureLibrary = TextureLibrary::GetInstance();

        textureLibrary.AddTexture("viking room", "assets/textures/viking_room.png");
    }

    void SceneAsciiPostProcessing::InitializeMaterials() {
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
    }

    void SceneAsciiPostProcessing::ConfigureModels() {
        MaterialLibrary& materialLibrary = MaterialLibrary::GetInstance();

        Model* vikingRoom = _modelManager.AddModelFromFile("viking room", "assets/models/viking_room.obj");
        vikingRoom->AddMaterial(materialLibrary.GetMaterialInstance("SingleColor"));
        vikingRoom->AddMaterial(materialLibrary.GetMaterialInstance("Texture"));

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

    void SceneAsciiPostProcessing::RenderWithShader(Shader *shaderProgram) {
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

    void SceneAsciiPostProcessing::RenderFSQ() {
        _fsq.Bind();
        Backend::Rendering::DrawIndexed(_fsq.GetVAO(), _fsq.GetRenderingPrimitive());
        _fsq.Unbind();
    }

    void SceneAsciiPostProcessing::ConstructAsciiMaps() {
        // .
        CharacterBitmap period(5, 5);
        period.SetBit(12);
        _characterMap.AddCharacter(period);

        // :
        CharacterBitmap semicolon(5, 5);
        semicolon.SetBit(6);
        semicolon.SetBit(16);
        _characterMap.AddCharacter(semicolon);

        // *
        CharacterBitmap asterisk(5, 5);
        asterisk.SetBit(2);
        asterisk.SetBit(5);
        asterisk.SetBit(6);
        asterisk.SetBit(7);
        asterisk.SetBit(8);
        asterisk.SetBit(9);
        asterisk.SetBit(12);
        asterisk.SetBit(16);
        asterisk.SetBit(18);
        _characterMap.AddCharacter(asterisk);

        // o
        CharacterBitmap o(5, 5);
        o.SetBit(1);
        o.SetBit(2);
        o.SetBit(3);
        o.SetBit(5);
        o.SetBit(9);
        o.SetBit(10);
        o.SetBit(14);
        o.SetBit(15);
        o.SetBit(19);
        o.SetBit(21);
        o.SetBit(22);
        o.SetBit(23);
        _characterMap.AddCharacter(o);
    }

}