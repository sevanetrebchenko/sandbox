
#include <scenes/project1/project1.h>
#include <framework/imgui_log.h>
#include <framework/backend.h>
#include <framework/material_library.h>
#include <framework/shader_library.h>
#include <framework/texture_library.h>
#include <framework/primitive_loader.h>
#include <framework/imgui_log.h>

namespace Sandbox {

    SceneProject1::SceneProject1(int width, int height) : Scene("Animation", width, height),
                                                                   _fbo(2560, 1440) {
        _dataDirectory = "data/scenes/project1";
    }

    SceneProject1::~SceneProject1() {
    }

    void SceneProject1::OnInit() {
        InitializeShaders();
        InitializeTextures();
        InitializeMaterials();

        ConfigureLights();
        ConstructFBO();
        ConfigureModels();
    }

    void SceneProject1::OnUpdate(float dt) {
        ShaderLibrary& shaderLibrary = ShaderLibrary::GetInstance();
        ImGuiLog& log = ImGuiLog::GetInstance();

        // Recompile shaders.
        try {
            shaderLibrary.RecompileAllModified();
        }
        catch (std::runtime_error& err) {
            log.LogError("Shader recompilation failed: %s", err.what());
        }
    }

    void SceneProject1::OnPreRender() {
        Backend::Core::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SceneProject1::OnRender() {
        ShaderLibrary& shaderLibrary = ShaderLibrary::GetInstance();

        _fbo.BindForReadWrite();
        Backend::Core::SetViewport(0, 0, _fbo.GetWidth(), _fbo.GetHeight());

        _fbo.DrawBuffers();
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Shader* phongShader = shaderLibrary.GetShader("Phong");
        phongShader->Bind();

        // Set camera uniforms.
        phongShader->SetUniform("cameraTransform", _camera.GetMatrix());
        phongShader->SetUniform("cameraPosition", _camera.GetEyePosition());
        phongShader->SetUniform("viewTransform", _camera.GetViewMatrix());
        phongShader->SetUniform("cameraNearPlane", _camera.GetNearPlaneDistance());
        phongShader->SetUniform("cameraFarPlane", _camera.GetFarPlaneDistance());

        for (Model* model : _modelManager.GetModels()) {
            Transform& transform = model->GetTransform();
            Mesh* mesh = model->GetMesh();
            Material* material = model->GetMaterial("Phong");

            // Pre render stage.
            if (material) {
                const glm::mat4& modelTransform = transform.GetMatrix();
                phongShader->SetUniform("modelTransform", modelTransform);
                phongShader->SetUniform("normalTransform", glm::inverse(modelTransform));

                // Bind all related uniforms with this shader.
                material->Bind(phongShader);
            }

            // Render stage.
            mesh->Bind();
            Backend::Rendering::DrawIndexed(mesh->GetVAO(), mesh->GetRenderingPrimitive());
            mesh->Unbind();

            // Post render stage.
        }

        phongShader->Unbind();

        _fbo.Unbind();
        Backend::Core::SetViewport(0, 0, _window.GetWidth(), _window.GetHeight());
    }

    void SceneProject1::OnPostRender() {
        // Restore viewport.

    }

    void SceneProject1::OnImGui() {
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

        // ImGui log output.
        log.OnImGui();

        // Materials.
        materialLibrary.OnImGui();

        _modelManager.OnImGui();
    }

    void SceneProject1::OnShutdown() {

    }

    void SceneProject1::InitializeShaders() {
        ShaderLibrary& shaderLibrary = ShaderLibrary::GetInstance();

        shaderLibrary.AddShader("Phong", { "assets/shaders/phong_shading.vert", "assets/shaders/phong_shading.frag" });
        shaderLibrary.AddShader("FSQ", { "assets/shaders/fsq.vert", "assets/shaders/fsq.frag" });

//        shaderLibrary.AddShader("SingleColor", { "assets/shaders/color.vert", "assets/shaders/color.frag" });
//        shaderLibrary.AddShader("Texture", { "assets/shaders/texture.vert", "assets/shaders/texture.frag" });
//
//        shaderLibrary.AddShader("Depth", { "assets/shaders/fsq.vert", "assets/shaders/depth.frag" });
//        shaderLibrary.AddShader("FSQ", { "assets/shaders/fsq.vert", "assets/shaders/fsq.frag" });
//        shaderLibrary.AddShader("DeferredPhongShading", { "assets/shaders/fsq.vert", "assets/shaders/deferred_phong_shading.frag" });
//        shaderLibrary.AddShader("GeometryPass", { "assets/shaders/geometry_buffer.vert", "assets/shaders/geometry_buffer.frag" });
    }

    void SceneProject1::InitializeTextures() {
        TextureLibrary& textureLibrary = TextureLibrary::GetInstance();

        textureLibrary.AddTexture("viking room", "assets/textures/viking_room.png");
    }

    void SceneProject1::InitializeMaterials() {
        MaterialLibrary& materialLibrary = MaterialLibrary::GetInstance();
        TextureLibrary& textureLibrary = TextureLibrary::GetInstance();

        // Single color material.
        Material* singleColorMaterial = new Material("SingleColor", {
            { "surfaceColor", glm::vec3(1.0f) }
        });
        singleColorMaterial->GetUniform("surfaceColor")->UseColorPicker(true);
        materialLibrary.AddMaterial(singleColorMaterial);

        // Textured material.
        Material* textureMaterial = new Material("Texture", {
            { "modelTexture", TextureSampler(textureLibrary.GetTexture("viking room"), 0) }
        });
        materialLibrary.AddMaterial(textureMaterial);

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
        materialLibrary.AddMaterial(phongMaterial);
    }

    void SceneProject1::ConfigureModels() {
        MaterialLibrary& materialLibrary = MaterialLibrary::GetInstance();

        Model* bunny = _modelManager.AddModelFromFile("bunny", "assets/models/bunny_high_poly.obj");
        Material* bunnyMaterial = materialLibrary.GetMaterialInstance("Phong");
        bunnyMaterial->GetUniform("ambientCoefficient")->SetData(glm::vec3(0.05f));
        bunny->AddMaterial(bunnyMaterial);
    }

    void SceneProject1::ConstructFBO() {
        _fbo.BindForReadWrite();

        // Output texture.
        Texture* outputTexture = new Texture("output");
        outputTexture->Bind();
        outputTexture->ReserveData(Texture::AttachmentType::COLOR, 2560, 1440);
        outputTexture->Unbind();
        _fbo.AttachRenderTarget(outputTexture);

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

    void SceneProject1::ConfigureLights() {
        Light one;
        Transform& oneTransform = one.GetTransform();
        oneTransform.SetPosition(glm::vec3(2.0f, 0.0f, 0.0f));
        _lightingManager.AddLight(one);

        Light two;
        Transform& twoTransform = two.GetTransform();
        twoTransform.SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
        _lightingManager.AddLight(two);

        Light three;
        Transform& threeTransform = three.GetTransform();
        threeTransform.SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
        _lightingManager.AddLight(three);
    }

}
