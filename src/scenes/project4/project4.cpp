
#include <scenes/project4/project4.h>
#include <framework/imgui_log.h>
#include <framework/backend.h>
#include <framework/material_library.h>
#include <framework/shader_library.h>
#include <framework/texture_library.h>
#include <framework/primitive_loader.h>
#include <framework/imgui_log.h>
#include <framework/skinned_mesh.h>
#include <framework/animated_model.h>
#include <framework/animator.h>

namespace Sandbox {

    SceneProject4::SceneProject4(int width, int height) : Scene("Physically-Based Modeling", width, height),
                                                          _fbo(1920, 1080) {
        _dataDirectory = "data/scenes/project4";
    }

    SceneProject4::~SceneProject4() {
    }

    void SceneProject4::OnInit() {
        InitializeShaders();
        InitializeTextures();
        InitializeMaterials();

        ConfigureLights();
        ConstructFBO();
        ConfigureModels();

        // Initialize debug renderer.
        _debugRenderer = new DDRenderInterfaceCoreGL();
        dd::initialize(_debugRenderer);

        // Initialize camera position.
        _camera.SetEyePosition(glm::vec3(-4.0f, 2.0f, 4.0f));
        _camera.SetLookAtDirection(glm::normalize(glm::vec3(2.5f, -1.5f, -2.5f)));
    }

    void SceneProject4::OnUpdate(float dt) {
        ShaderLibrary &shaderLibrary = ShaderLibrary::GetInstance();
        ImGuiLog &log = ImGuiLog::GetInstance();

        // Recompile shaders.
        try {
            shaderLibrary.RecompileAllModified();
        }
        catch (std::runtime_error &err) {
            log.LogError("Shader recompilation failed: %s", err.what());
        }

        _modelManager.Update(dt);

        rb_.Update(dt);
    }

    void SceneProject4::OnPreRender() {
        Backend::Core::ClearColor(glm::vec4(20.0f, 30.0f, 80.0f, 255.0f) / glm::vec4(255.0f));
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SceneProject4::OnRender() {
        ShaderLibrary &shaderLibrary = ShaderLibrary::GetInstance();

        _fbo.BindForReadWrite();
        Backend::Core::SetViewport(0, 0, _fbo.GetWidth(), _fbo.GetHeight());

        _fbo.DrawBuffers();
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Shader* shader = shaderLibrary.GetShader("Phong");
        shader->Bind();

        // Set camera uniforms.
        shader->SetUniform("cameraTransform", _camera.GetMatrix());
        shader->SetUniform("cameraPosition", _camera.GetEyePosition());
        shader->SetUniform("viewTransform", _camera.GetViewMatrix());
        shader->SetUniform("cameraNearPlane", _camera.GetNearPlaneDistance());
        shader->SetUniform("cameraFarPlane", _camera.GetFarPlaneDistance());

        // Render cubes at the mass points.
        for (RigidBody& rigidBody : rb_.structure_) {
        	Transform &transform = rigidBody.model_.GetTransform();
        	const Mesh *mesh = rigidBody.model_.GetMesh();
        	Material* material = rigidBody.model_.GetMaterial("Phong");

        	if (material) {
        		const glm::mat4 &modelTransform = transform.GetMatrix();
        		shader->SetUniform("modelTransform", modelTransform);
        		shader->SetUniform("normalTransform", glm::inverse(modelTransform));

        		// Bind all related uniforms with this shader.
        		material->Bind(shader);
        	}

        	// Render stage.
        	mesh->Bind();
        	Backend::Rendering::DrawIndexed(mesh->GetVAO(), mesh->GetRenderingPrimitive());
        	mesh->Unbind();

        	// Post render stage.
        }

        shader->Unbind();

        Backend::Core::EnableFlag(GL_LINE_SMOOTH);
        glLineWidth(2.0f);

        rb_.Render();

        // Debug drawing.
        glUseProgram(_debugRenderer->linePointProgram);
        _debugRenderer->mvpMatrix = _camera.GetMatrix();

        // Grid.
        ddVec3 gray = {0.5f, 0.5f, 0.5f};
        dd::xzSquareGrid(-20.0f, 20.0f, 0.0f, 1.f, &gray[0]);

        glUseProgram(0);

        // Flush debug renderer.
        const double seconds = glfwGetTime();
        dd::flush(static_cast<std::int64_t>(seconds * 1000.0));

        Backend::Core::EnableFlag(GL_DEPTH_TEST);

        _fbo.Unbind();
        Backend::Core::SetViewport(0, 0, _window.GetWidth(), _window.GetHeight());
    }

    void SceneProject4::OnPostRender() {
        // Restore viewport.
    }

    void SceneProject4::OnImGui() {
        ShaderLibrary &shaderLibrary = ShaderLibrary::GetInstance();
        MaterialLibrary &materialLibrary = MaterialLibrary::GetInstance();
        ImGuiLog &log = ImGuiLog::GetInstance();

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
            ImGui::SetCursorPosY(ImGui::GetItemRectSize().y +
                                 (ImGui::GetWindowSize().y - ImGui::GetItemRectSize().y - imageSize.y) * 0.5f);

            ImGui::Image(reinterpret_cast<ImTextureID>(_fbo.GetNamedRenderTarget("output")->ID()), imageSize,
                         ImVec2(0, 1), ImVec2(1, 0));
        }

        ImGui::End();

        // ImGui log output.
        log.OnImGui();

        rb_.OnImGui();
    }

    void SceneProject4::OnShutdown() {
        dd::shutdown();
    }

    void SceneProject4::InitializeShaders() {
        ShaderLibrary &shaderLibrary = ShaderLibrary::GetInstance();

        shaderLibrary.AddShader("Phong", {"assets/shaders/phong_shading.vert", "assets/shaders/phong_shading.frag"});
        shaderLibrary.AddShader("FSQ", {"assets/shaders/fsq.vert", "assets/shaders/fsq.frag"});

        //        shaderLibrary.AddShader("SingleColor", { "assets/shaders/color.vert", "assets/shaders/color.frag" });
        //        shaderLibrary.AddShader("Texture", { "assets/shaders/texture.vert", "assets/shaders/texture.frag" });
        //
        //        shaderLibrary.AddShader("Depth", { "assets/shaders/fsq.vert", "assets/shaders/depth.frag" });
        //        shaderLibrary.AddShader("FSQ", { "assets/shaders/fsq.vert", "assets/shaders/fsq.frag" });
        //        shaderLibrary.AddShader("DeferredPhongShading", { "assets/shaders/fsq.vert", "assets/shaders/deferred_phong_shading.frag" });
        //        shaderLibrary.AddShader("GeometryPass", { "assets/shaders/geometry_buffer.vert", "assets/shaders/geometry_buffer.frag" });
    }

    void SceneProject4::InitializeTextures() {
        TextureLibrary &textureLibrary = TextureLibrary::GetInstance();
    }

    void SceneProject4::InitializeMaterials() {
        MaterialLibrary &materialLibrary = MaterialLibrary::GetInstance();
        TextureLibrary &textureLibrary = TextureLibrary::GetInstance();

        // Single color material.
        Material *singleColorMaterial = new Material("SingleColor", {
                {"surfaceColor", glm::vec3(1.0f)}
        });
        singleColorMaterial->GetUniform("surfaceColor")->UseColorPicker(true);
        materialLibrary.AddMaterial(singleColorMaterial);

        // Textured material.
        Material *textureMaterial = new Material("Texture", {
                {"modelTexture", TextureSampler(textureLibrary.GetTexture("viking room"), 0)}
        });
        materialLibrary.AddMaterial(textureMaterial);

        // Phong shading material.
        Material *phongMaterial = new Material("Phong", {
            {"ambientCoefficient",  glm::vec3(0.5f)},
            {"diffuseCoefficient",  glm::vec3(0.5f)},
            {"specularCoefficient", glm::vec3(1.0f)},
            {"specularExponent",    50.0f}
        });
        phongMaterial->GetUniform("ambientCoefficient")->UseColorPicker(true);
        phongMaterial->GetUniform("diffuseCoefficient")->UseColorPicker(true);
        phongMaterial->GetUniform("specularCoefficient")->UseColorPicker(true);
        phongMaterial->GetUniform("specularExponent")->SetSliderRange(0.0f, 100.0f);
        materialLibrary.AddMaterial(phongMaterial);
    }

    void SceneProject4::ConfigureModels() {
//        MaterialLibrary &materialLibrary = MaterialLibrary::GetInstance();
//
//        // Sphere - small target object.
//        Model *sphere = _modelManager.AddModelFromFile("cube", "assets/models/cube2.obj");
//
//        sphere->GetTransform().SetScale(glm::vec3(2.0f));
//
//        Material *material = materialLibrary.GetMaterialInstance("Phong");
//        material->GetUniform("ambientCoefficient")->SetData(glm::vec3(0.08f));
//        material->GetUniform("diffuseCoefficient")->SetData(glm::vec3(0.3f));
//        material->GetUniform("specularCoefficient")->SetData(glm::vec3(0.85f));
//        sphere->AddMaterial(material);

        // Configure shape of rigid body internals.
        rb_.Preallocate(glm::vec3(3.0f));
//        RigidBody anchor { glm::vec3(0.0f, 1.0f, 0.0f) };
//        anchor.SetFixed(true);
//
//        rb_.structure_.emplace_back(anchor);
//        rb_.structure_.emplace_back();
//
//        rb_.connections_.emplace_back(&rb_.structure_[0], &rb_.structure_[1]);
    }

    void SceneProject4::ConstructFBO() {
        _fbo.BindForReadWrite();

        // Output texture.
        Texture *outputTexture = new Texture("output");
        outputTexture->Bind();
        outputTexture->ReserveData(Texture::AttachmentType::COLOR, 1920, 1080);
        outputTexture->Unbind();
        _fbo.AttachRenderTarget(outputTexture);

        // Depth buffer (RBO).
        RenderBufferObject *depthBuffer = new RenderBufferObject();
        depthBuffer->Bind();
        depthBuffer->ReserveData(1920, 1080);
        _fbo.AttachDepthBuffer(depthBuffer);

        if (!_fbo.CheckStatus()) {
            throw std::runtime_error("Custom FBO is not complete.");
        }

        _fbo.Unbind();
    }

    void SceneProject4::ConfigureLights() {
        {
            Light light;
            Transform &transform = light.GetTransform();
            transform.SetPosition(glm::vec3(0.0f, 20.0f, 0.0f));
            _lightingManager.AddLight(light);
        }
    }

}
