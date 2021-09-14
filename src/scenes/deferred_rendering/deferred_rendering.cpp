
#include <scenes/deferred_rendering/deferred_rendering.h>
#include <framework/imgui_log.h>
#include <framework/backend.h>
#include <framework/material_library.h>
#include <framework/shader_library.h>
#include <framework/texture_library.h>
#include <framework/primitive_loader.h>
#include <framework/imgui_log.h>

namespace Sandbox {

    SceneDeferredRendering::SceneDeferredRendering() : IScene(),
                                                       _fbo(2560, 1440) {
    }

    SceneDeferredRendering::~SceneDeferredRendering() {
    }

    void SceneDeferredRendering::OnInit() {
        InitializeShaders();
        InitializeMaterials();

        ConfigureLights();
        ConstructFBO();
        ConfigureModels();
    }

    void SceneDeferredRendering::OnUpdate(float dt) {
        ShaderLibrary* shaderLibrary = Singleton<ShaderLibrary>::GetInstance();
        ImGuiLog* log = Singleton<ImGuiLog>::GetInstance();

        // Recompile shaders.
        try {
            shaderLibrary->RecompileAllModified();
        }
        catch (std::runtime_error& err) {
            log->LogError("Shader recompilation failed: %s", err.what());
        }
    }

    void SceneDeferredRendering::OnPreRender() {
        Backend::Core::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SceneDeferredRendering::OnRender() {
        Window* window = Singleton<Window>::GetInstance();

        // Set viewport.
        _fbo.BindForReadWrite();
        Backend::Core::SetViewport(0, 0, _fbo.GetWidth(), _fbo.GetHeight());

        GeometryPass();

        Backend::Core::WriteDepth(false); // Do not write depth from future passes (preserve depth of actual scene, not FSQ).

        RenderOutputScene();
        RenderDepthBuffer();

        Backend::Core::WriteDepth(true); // Return depth-writing back to normal.

        // Restore viewport.
        _fbo.Unbind();
        Backend::Core::SetViewport(0, 0, window->GetWidth(), window->GetHeight());
    }

    void SceneDeferredRendering::OnPostRender() {

    }

    void SceneDeferredRendering::OnImGui() {
        MaterialLibrary* materialLibrary = Singleton<MaterialLibrary>::GetInstance();
        Window* window = Singleton<Window>::GetInstance();

        // Draw the final output from the hybrid rendering pipeline.
        if (ImGui::Begin("Framebuffer")) {
            // Ensure proper scene image scaling.
            float maxWidth = ImGui::GetWindowContentRegionWidth();
            float aspect = static_cast<float>(window->GetWidth()) / static_cast<float>(window->GetHeight());
            ImVec2 imageSize = ImVec2(maxWidth, maxWidth / aspect);
            ImGui::SetCursorPosY(ImGui::GetItemRectSize().y + (ImGui::GetWindowSize().y - ImGui::GetItemRectSize().y - imageSize.y) * 0.5f);

            ImGui::Image(reinterpret_cast<ImTextureID>(_fbo.GetNamedRenderTarget("output")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
        }

        ImGui::End();

        // Draw individual deferred rendering textures.
        if (ImGui::Begin("Debug Textures")) {
            // Ensure proper scene image scaling.
            float maxWidth = ImGui::GetWindowContentRegionWidth();
            float aspect = static_cast<float>(window->GetWidth()) / static_cast<float>(window->GetHeight());
            ImVec2 imageSize = ImVec2(maxWidth, maxWidth / aspect);

            ImGui::Text("Position (view space):");
            ImGui::Image(reinterpret_cast<ImTextureID>(_fbo.GetNamedRenderTarget("position")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Normals (view space):");
            ImGui::Image(reinterpret_cast<ImTextureID>(_fbo.GetNamedRenderTarget("normal")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Ambient component:");
            ImGui::Image(reinterpret_cast<ImTextureID>(_fbo.GetNamedRenderTarget("ambient")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Diffuse component:");
            ImGui::Image(reinterpret_cast<ImTextureID>(_fbo.GetNamedRenderTarget("diffuse")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Specular component:");
            ImGui::Image(reinterpret_cast<ImTextureID>(_fbo.GetNamedRenderTarget("specular")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();

            ImGui::Text("Scene depth:");
            ImGui::Image(reinterpret_cast<ImTextureID>(_fbo.GetNamedRenderTarget("depthTexture")->ID()), imageSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Separator();
        }
        ImGui::End();
    }

    void SceneDeferredRendering::OnShutdown() {

    }

    void SceneDeferredRendering::InitializeShaders() {
        ShaderLibrary* shaderLibrary = Singleton<ShaderLibrary>::GetInstance();

        shaderLibrary->AddShader("SingleColor", { "assets/shaders/color.vert", "assets/shaders/color.frag" });
        shaderLibrary->AddShader("Texture", { "assets/shaders/texture.vert", "assets/shaders/texture.frag" });

        shaderLibrary->AddShader("Depth", { "assets/shaders/fsq.vert", "assets/shaders/depth.frag" });
        shaderLibrary->AddShader("FSQ", { "assets/shaders/fsq.vert", "assets/shaders/fsq.frag" });
        shaderLibrary->AddShader("DeferredPhongShading", { "assets/shaders/fsq.vert", "assets/shaders/deferred_phong_shading.frag" });
        shaderLibrary->AddShader("GeometryPass", { "assets/shaders/geometry_buffer.vert", "assets/shaders/geometry_buffer.frag" });
    }

    void SceneDeferredRendering::InitializeMaterials() {
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
        materialLibrary->AddMaterial(phongMaterial);
    }

    void SceneDeferredRendering::ConfigureModels() {
        MaterialLibrary* materialLibrary = Singleton<MaterialLibrary>::GetInstance();
        ModelManager* modelManager = Singleton<ModelManager>::GetInstance();

        Model* bunny = modelManager->AddModelFromFile("bunny", "assets/models/bunny_high_poly.obj");
        Material* bunnyMaterial = materialLibrary->GetMaterialInstance("Phong");
        bunnyMaterial->GetUniform("ambientCoefficient")->SetData(glm::vec3(0.05f));
        bunny->AddMaterial(bunnyMaterial);
    }

    void SceneDeferredRendering::ConstructFBO() {
        _fbo.BindForReadWrite();

        // Position (view space).
        Texture* positionTexture = new Texture("position");
        positionTexture->Bind();
        positionTexture->ReserveData(Texture::AttachmentType::COLOR, 2560, 1440);
        positionTexture->Unbind();
        _fbo.AttachRenderTarget(positionTexture);

        // Normals (view space).
        Texture* normalTexture = new Texture("normal");
        normalTexture->Bind();
        normalTexture->ReserveData(Texture::AttachmentType::COLOR, 2560, 1440);
        normalTexture->Unbind();
        _fbo.AttachRenderTarget(normalTexture);

        // Ambient color.
        Texture* ambientTexture = new Texture("ambient");
        ambientTexture->Bind();
        ambientTexture->ReserveData(Texture::AttachmentType::COLOR, 2560, 1440);
        ambientTexture->Unbind();
        _fbo.AttachRenderTarget(ambientTexture);

        // Diffuse color.
        Texture* diffuseTexture = new Texture("diffuse");
        diffuseTexture->Bind();
        diffuseTexture->ReserveData(Texture::AttachmentType::COLOR, 2560, 1440);
        diffuseTexture->Unbind();
        _fbo.AttachRenderTarget(diffuseTexture);

        // Specular color.
        Texture* specularTexture = new Texture("specular");
        specularTexture->Bind();
        specularTexture->ReserveData(Texture::AttachmentType::COLOR, 2560, 1440);
        specularTexture->Unbind();
        _fbo.AttachRenderTarget(specularTexture);

        // Output texture.
        Texture* outputTexture = new Texture("output");
        outputTexture->Bind();
        outputTexture->ReserveData(Texture::AttachmentType::COLOR, 2560, 1440);
        outputTexture->Unbind();
        _fbo.AttachRenderTarget(outputTexture);

        // Depth texture.
        Texture* depthTexture = new Texture("depthTexture");
        depthTexture->Bind();
        depthTexture->ReserveData(Texture::AttachmentType::COLOR, 2560, 1440);
        depthTexture->Unbind();
        _fbo.AttachRenderTarget(depthTexture);

        // Depth buffer.
        Texture* depthBuffer = new Texture("depthBuffer");
        depthBuffer->Bind();
        depthBuffer->ReserveData(Texture::AttachmentType::DEPTH, 2560, 1440);
        depthBuffer->Unbind();
        _fbo.AttachRenderTarget(depthBuffer);

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

    void SceneDeferredRendering::ConfigureLights() {
        LightingManager* lightingManager = Singleton<LightingManager>::GetInstance();

        Light one;
        Transform& oneTransform = one.GetTransform();
        oneTransform.SetPosition(glm::vec3(2.0f, 0.0f, 0.0f));
        lightingManager->AddLight(one);

        Light two;
        Transform& twoTransform = two.GetTransform();
        twoTransform.SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
        lightingManager->AddLight(two);


        Light three;
        Transform& threeTransform = three.GetTransform();
        threeTransform.SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
        lightingManager->AddLight(three);
    }

    void SceneDeferredRendering::GeometryPass() {
        ShaderLibrary* shaderLibrary = Singleton<ShaderLibrary>::GetInstance();
        ModelManager* modelManager = Singleton<ModelManager>::GetInstance();

        _fbo.DrawBuffers(0, 5);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear everything for a new scene.

        Shader* geometryShader = shaderLibrary->GetShader("GeometryPass");
        geometryShader->Bind();

        // Set camera uniforms.
        geometryShader->SetUniform("cameraTransform", camera_.GetMatrix());
        geometryShader->SetUniform("viewTransform", camera_.GetViewMatrix());

        // Render models to FBO attachments.
        for (Model* model : modelManager->GetModels()) {
            Transform& transform = model->GetTransform();
            Mesh& mesh = model->GetMesh();
            Material* material = model->GetMaterial("Phong");

            // Pre render stage.
            if (material) {
                const glm::mat4& modelTransform = transform.GetMatrix();
                geometryShader->SetUniform("modelTransform", modelTransform);

                // Bind all related uniforms with this shader.
                material->Bind(geometryShader);
            }

            // Render stage.
            mesh.Bind();
            Backend::Rendering::DrawIndexed(mesh.GetVAO(), mesh.GetRenderingPrimitive());
            mesh.Unbind();

            // Post render stage.
        }

        geometryShader->Unbind();
    }

    void SceneDeferredRendering::RenderOutputScene() {
        ShaderLibrary* shaderLibrary = Singleton<ShaderLibrary>::GetInstance();

        // Render to 'output' texture.
        _fbo.DrawBuffers(5, 1);
        //Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT); // We are not touching depth buffer here.

        Shader* deferredPhongShader = shaderLibrary->GetShader("DeferredPhongShading");
        deferredPhongShader->Bind();

        // Set camera uniforms.
        deferredPhongShader->SetUniform("cameraPosition", camera_.GetEyePosition());
        deferredPhongShader->SetUniform("cameraTransform", camera_.GetMatrix());
        deferredPhongShader->SetUniform("viewTransform", camera_.GetViewMatrix());
        deferredPhongShader->SetUniform("cameraNearPlane", camera_.GetNearPlaneDistance());
        deferredPhongShader->SetUniform("cameraFarPlane", camera_.GetFarPlaneDistance());

        // Bind geometry pass textures.
        Backend::Rendering::BindTextureWithSampler(deferredPhongShader, _fbo.GetNamedRenderTarget("position"), 0);
        Backend::Rendering::BindTextureWithSampler(deferredPhongShader, _fbo.GetNamedRenderTarget("normal"), 1);
        Backend::Rendering::BindTextureWithSampler(deferredPhongShader, _fbo.GetNamedRenderTarget("ambient"), 2);
        Backend::Rendering::BindTextureWithSampler(deferredPhongShader, _fbo.GetNamedRenderTarget("diffuse"), 3);
        Backend::Rendering::BindTextureWithSampler(deferredPhongShader, _fbo.GetNamedRenderTarget("specular"), 4);

        // Render to output texture using FSQ.
        Backend::Rendering::DrawFSQ();

        deferredPhongShader->Unbind();
    }

    void SceneDeferredRendering::RenderDepthBuffer() {
        ShaderLibrary* shaderLibrary = Singleton<ShaderLibrary>::GetInstance();

        // Render to depth texturing using FSQ.
        _fbo.DrawBuffers(6, 1);
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT);

        Shader* depthShader = shaderLibrary->GetShader("Depth");
        depthShader->Bind();

        // Uniforms.
        depthShader->SetUniform("cameraNearPlane", 0.5f);
        depthShader->SetUniform("cameraFarPlane", 500.0f);
        Backend::Rendering::BindTextureWithSampler(depthShader, _fbo.GetNamedRenderTarget("depthBuffer"), "inputTexture", 0);

        // Render to depth texture using FSQ.
        Backend::Rendering::DrawFSQ();

        depthShader->Unbind();
    }

}
