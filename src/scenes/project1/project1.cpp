
#include <scenes/project1/project1.h>
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

        // Initialize debug renderer.
        _debugRenderer = new DDRenderInterfaceCoreGL();
        dd::initialize(_debugRenderer);

        // Initialize camera position.
        _camera.SetEyePosition(glm::vec3(2.5f));
        _camera.SetLookAtDirection(glm::normalize(glm::vec3(-2.5f, -1.0f, -2.5f)));
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

        _modelManager.Update(dt);
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
            const Mesh* mesh = model->GetMesh();
            Material* material = model->GetMaterial("Phong");

            // Pre render stage.
            if (material) {
                const glm::mat4& modelTransform = transform.GetMatrix();
                phongShader->SetUniform("modelTransform", modelTransform);
                phongShader->SetUniform("normalTransform", glm::inverse(modelTransform));

                // Bind all related uniforms with this shader.
                material->Bind(phongShader);
            }

            if (AnimatedModel* animatedModel = dynamic_cast<AnimatedModel*>(model); animatedModel) {
                // Bind animated model uniforms.
                Animator* animator = animatedModel->GetAnimator();
                const std::vector<VQS>& boneTransforms = animator->GetFinalBoneTransformations();
                int numBones = boneTransforms.size();

                phongShader->SetUniform("numBones", numBones);

                for (int i = 0; i < numBones; ++i) {
                    const VQS& vqs = boneTransforms[i];
                    phongShader->SetUniform("finalBoneTransformations[" + std::to_string(i) + "].translation", vqs.GetTranslation());
                    phongShader->SetUniform("finalBoneTransformations[" + std::to_string(i) + "].rotation", vqs.GetOrientation().ToVec4());
                    phongShader->SetUniform("finalBoneTransformations[" + std::to_string(i) + "].scale", vqs.GetScalingFactor());
                }
            }

            // Render stage.
            mesh->Bind();
            Backend::Rendering::DrawIndexed(mesh->GetVAO(), mesh->GetRenderingPrimitive());
            mesh->Unbind();

            // Post render stage.
        }

        phongShader->Unbind();


        // Debug drawing.
        glUseProgram(_debugRenderer->linePointProgram);
        _debugRenderer->mvpMatrix = _camera.GetMatrix();

        // Grid.
        ddVec3_In cGray = { 0.15f, 0.15f, 0.15f };
        dd::xzSquareGrid(-20.0f, 20.0f, 0.0f, 1.f, &cGray[0], 0, true);

        // Model skeleton.
        for (Model* model : _modelManager.GetModels()) {
            if (AnimatedModel* animatedModel = dynamic_cast<AnimatedModel*>(model); animatedModel) {
                RenderSkeletonBones(animatedModel);
                break;
            }
        }

        glUseProgram(0);

        // Flush debug renderer.
        const double seconds = glfwGetTime();
        dd::flush(static_cast<std::int64_t>(seconds * 1000.0));

        Backend::Core::EnableFlag(GL_DEPTH_TEST);

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
        dd::shutdown();
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

        AnimatedModel* walkingMan = dynamic_cast<AnimatedModel*>(_modelManager.AddModelFromFile("walking man", "assets/models/CesiumMan.glb"));
        Material* material = materialLibrary.GetMaterialInstance("Phong");
        material->GetUniform("ambientCoefficient")->SetData(glm::vec3(0.05f));
        material->GetUniform("diffuseCoefficient")->SetData(glm::vec3(0.3f));
        walkingMan->AddMaterial(material);

        // Values hard-coded for this model.
        walkingMan->GetTransform().SetRotation(glm::vec3(270.0f, 0.0f, 0.0f));
        walkingMan->GetTransform().SetScale(glm::vec3(2.0f));

        walkingMan->GetAnimator()->PlayAnimation(0);
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

    void SceneProject1::RenderSkeletonBones(AnimatedModel *animatedModel) const {
        Skeleton* skeleton = animatedModel->GetSkeleton();
        Animator* animator = animatedModel->GetAnimator();

        if (skeleton->_drawSkeleton) {
            for (int root : skeleton->_roots) {
                RenderSkeletonBone(skeleton, animator, animatedModel->GetTransform().GetMatrix(), animatedModel->GetTransform().GetPosition(), root);
            }
        }
    }

    void SceneProject1::RenderSkeletonBone(Skeleton *skeleton, Animator *animator, const glm::mat4 &parentTransform, const glm::vec3 &origin, int root) const {
        const std::vector<VQS>& finalTransformations = animator->GetFinalBoneTransformations();

        glm::vec3 start = origin;
        glm::vec3 end = glm::vec3(parentTransform * glm::vec4(finalTransformations[root] * skeleton->_bones[root]._modelToBoneVQS.GetTranslation(), 1.0f));
        glm::vec3 direction = start - end;

        // Don't render the base root node.
        bool isBoneBaseRoot = false;
        for (int currentID : skeleton->_roots) {
            if (root == currentID) {
                isBoneBaseRoot = true;
                break;
            }
        }

        // Render bone.
        if (!isBoneBaseRoot) {
            dd::line(static_cast<const float*>(&start[0]), static_cast<const float*>(&end[0]), dd::colors::Orange, 0, false);
            dd::cone(static_cast<const float*>(&end[0]), static_cast<const float*>(&direction[0]), dd::colors::Orange, 0.025f, 0.0f, 0, false);
        }

        // Render child bones from the end of this bone.
        for (std::size_t i = 0; i < skeleton->_bones[root]._children.size(); ++i) {
            RenderSkeletonBone(skeleton, animator, parentTransform, end, skeleton->_bones[root]._children[i]);
        }
    }

}
