
#include <scenes/project3/project3.h>
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

    SceneProject3::SceneProject3(int width, int height) : Scene("Inverse Kinematics", width, height),
                                                          _fbo(1920, 1080) {
        _dataDirectory = "data/scenes/project3";
    }

    SceneProject3::~SceneProject3() {
    }

    void SceneProject3::OnInit() {
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

    void SceneProject3::OnUpdate(float dt) {
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

        static float accumulator = 0.0f;
        accumulator += dt;

        // Update target sphere position.
        Model* walkingMan = _modelManager.GetNamedModel("walking man");
        if (walkingMan) {
            AnimatedModel* animated = dynamic_cast<AnimatedModel*>(walkingMan);
            if (animated) {
                Animator* animator = animated->GetAnimator();
                Model* sphere = _modelManager.GetNamedModel("sphere");

                if (animator && sphere) {
                    sphere->GetTransform().SetPosition(animator->GetIKTargetPosition());

                    glm::vec3 pos = sphere->GetTransform().GetPosition();
//                    sphere->GetTransform().SetPosition(glm::vec3(pos.x, 3.0f * std::sin(accumulator), pos.z));

                    animator->SetIKTargetPosition(sphere->GetTransform().GetPosition());
                }
            }
        }
    }

    void SceneProject3::OnPreRender() {
        Backend::Core::ClearColor(glm::vec4(20.0f, 30.0f, 80.0f, 255.0f) / glm::vec4(255.0f));
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SceneProject3::OnRender() {
        ShaderLibrary &shaderLibrary = ShaderLibrary::GetInstance();

        _fbo.BindForReadWrite();
        Backend::Core::SetViewport(0, 0, _fbo.GetWidth(), _fbo.GetHeight());

        _fbo.DrawBuffers();
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        for (Model *model : _modelManager.GetModels()) {
            Transform &transform = model->GetTransform();
            const Mesh *mesh = model->GetMesh();

            Shader *shader;
            Material* material;

            AnimatedModel *animatedModel = dynamic_cast<AnimatedModel *>(model);

            if (animatedModel) {
                // Animated model has animated phong shader.
                shader = shaderLibrary.GetShader("Animated Phong");
                material = model->GetMaterial("Animated Phong");
            }
            else {
                shader = shaderLibrary.GetShader("Phong");
                material = model->GetMaterial("Phong");
            }

            shader->Bind();

            // Pre render stage.
            if (material) {
                const glm::mat4 &modelTransform = transform.GetMatrix();
                shader->SetUniform("modelTransform", modelTransform);
                shader->SetUniform("normalTransform", glm::inverse(modelTransform));

                // Bind all related uniforms with this shader.
                material->Bind(shader);
            }

            // Set camera uniforms.
            shader->SetUniform("cameraTransform", _camera.GetMatrix());
            shader->SetUniform("cameraPosition", _camera.GetEyePosition());
            shader->SetUniform("viewTransform", _camera.GetViewMatrix());
            shader->SetUniform("cameraNearPlane", _camera.GetNearPlaneDistance());
            shader->SetUniform("cameraFarPlane", _camera.GetFarPlaneDistance());

            // Bind animated model uniforms.
            if (animatedModel) {
                Animator *animator = animatedModel->GetAnimator();
                const std::vector<VQS> &boneTransforms = animator->GetBoneTransformations();
                int numBones = boneTransforms.size();

                shader->SetUniform("numBones", numBones);

                for (int i = 0; i < numBones; ++i) {
                    const VQS &vqs = boneTransforms[i];
                    shader->SetUniform("finalBoneTransformations[" + std::to_string(i) + "].translation",
                                       vqs.GetTranslation());
                    shader->SetUniform("finalBoneTransformations[" + std::to_string(i) + "].rotation",
                                       vqs.GetOrientation().ToVec4());
                    shader->SetUniform("finalBoneTransformations[" + std::to_string(i) + "].scale",
                                       vqs.GetScalingFactor());
                }
            }


            // Render stage.
            mesh->Bind();
            Backend::Rendering::DrawIndexed(mesh->GetVAO(), mesh->GetRenderingPrimitive());
            mesh->Unbind();

            // Post render stage.
            shader->Unbind();
        }

        // Debug drawing.
        glUseProgram(_debugRenderer->linePointProgram);
        _debugRenderer->mvpMatrix = _camera.GetMatrix();

        Backend::Core::EnableFlag(GL_LINE_SMOOTH);
        glLineWidth(2.0f);

        // Grid.
        ddVec3 gray = {0.5f, 0.5f, 0.5f};
        dd::xzSquareGrid(-20.0f, 20.0f, 0.0f, 1.f, &gray[0]);

        // Model skeleton.
        for (Model *model : _modelManager.GetModels()) {
            if (AnimatedModel *animatedModel = dynamic_cast<AnimatedModel *>(model); animatedModel) {
                RenderSkeletonBones(animatedModel);
                RenderAnimatedModelPath(animatedModel->GetPather());
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

    void SceneProject3::OnPostRender() {
        // Restore viewport.
    }

    void SceneProject3::OnImGui() {
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

        _modelManager.OnImGui();
    }

    void SceneProject3::OnShutdown() {
        dd::shutdown();
    }

    void SceneProject3::InitializeShaders() {
        ShaderLibrary &shaderLibrary = ShaderLibrary::GetInstance();

        shaderLibrary.AddShader("Animated Phong",
                                {"assets/shaders/phong_shading_animated.vert", "assets/shaders/phong_shading.frag"});
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

    void SceneProject3::InitializeTextures() {
        TextureLibrary &textureLibrary = TextureLibrary::GetInstance();
    }

    void SceneProject3::InitializeMaterials() {
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

        // Phong shading materials.
        {
            Material *phongMaterial = new Material("Animated Phong", {
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

        {
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
    }

    void SceneProject3::ConfigureModels() {
        MaterialLibrary &materialLibrary = MaterialLibrary::GetInstance();

        // Walking man - animated model.
        {
            AnimatedModel *walkingMan = dynamic_cast<AnimatedModel *>(_modelManager.AddModelFromFile("walking man",
                                                                                                     "assets/models/CesiumMan.glb"));
            Material *material = materialLibrary.GetMaterialInstance("Animated Phong");
            material->GetUniform("ambientCoefficient")->SetData(glm::vec3(0.08f));
            material->GetUniform("diffuseCoefficient")->SetData(glm::vec3(0.3f));
            material->GetUniform("specularCoefficient")->SetData(glm::vec3(0.85f));
            walkingMan->AddMaterial(material);

            // Values hard-coded for this model.
            walkingMan->GetTransform().SetRotation(270.0f, glm::vec3(1.0f, 0.0f, 0.0f));
            walkingMan->GetTransform().SetScale(glm::vec3(2.0f));

            walkingMan->GetAnimator()->PlayAnimation(0);

            walkingMan->SetPather(new Pather());
        }

        {
            // Sphere - small target object.
            Model *sphere = _modelManager.AddModelFromFile("sphere", "assets/models/sphere.obj");

            Material *material = materialLibrary.GetMaterialInstance("Phong");
            material->GetUniform("ambientCoefficient")->SetData(glm::vec3(0.08f));
            material->GetUniform("diffuseCoefficient")->SetData(glm::vec3(0.3f));
            material->GetUniform("specularCoefficient")->SetData(glm::vec3(0.85f));
            sphere->AddMaterial(material);

            sphere->GetTransform().SetScale(glm::vec3(0.2f));
        }
    }

    void SceneProject3::ConstructFBO() {
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

    void SceneProject3::ConfigureLights() {
        {
            Light light;
            Transform &transform = light.GetTransform();
            transform.SetPosition(glm::vec3(10.0f, 20.0f, 0.0f));
            _lightingManager.AddLight(light);
        }

        {
            Light light;
            Transform &transform = light.GetTransform();
            transform.SetPosition(glm::vec3(-10.0f, 20.0f, 0.0f));
            _lightingManager.AddLight(light);
        }

        {
            Light light;
            Transform &transform = light.GetTransform();
            transform.SetPosition(glm::vec3(0.0f, 20.0f, 10.0f));
            _lightingManager.AddLight(light);
        }

        {
            Light light;
            Transform &transform = light.GetTransform();
            transform.SetPosition(glm::vec3(0.0f, 20.0f, -10.0f));
            _lightingManager.AddLight(light);
        }

        {
            Light light;
            Transform &transform = light.GetTransform();
            transform.SetPosition(glm::vec3(0.0f, 10.0f, 0.0f));
            _lightingManager.AddLight(light);
        }

    }

    void SceneProject3::RenderSkeletonBones(AnimatedModel *animatedModel) const {
        Skeleton *skeleton = animatedModel->GetSkeleton();
        Animator *animator = animatedModel->GetAnimator();

        glLineWidth(3.0f);

        if (skeleton->_drawSkeleton) {
            for (int root : skeleton->_roots) {
                RenderSkeletonBone(skeleton, animator, animatedModel->GetTransform().GetMatrix(),
                                   animatedModel->GetTransform().GetPosition(), root, dd::colors::Orange);
            }

            // Clear hovered tags.
            for (Bone& bone : skeleton->_bones) {
                bone.hovered = false;
            }
        }
    }

    void SceneProject3::RenderSkeletonBone(Skeleton *skeleton, Animator *animator, const glm::mat4 &parentTransform,
                                           const glm::vec3 &origin, int root, const float color[3]) const {
        const std::vector<VQS> &finalTransformations = animator->GetBoneTransformations();

        const Bone& bone = skeleton->_bones[root];

        glm::vec3 start = origin;
        glm::vec3 end = glm::vec3(parentTransform * glm::vec4(finalTransformations[root] * bone._modelToBoneVQS.GetTranslation(), 1.0f));
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
            dd::sphere(static_cast<const float*>(&end[0]), color, 0.03f, 0, false);
            dd::cone(static_cast<const float *>(&end[0]), static_cast<const float *>(&direction[0]), color,
                     0.025f, 0.0f, 0, false);
        }

        if (bone.hovered) {
            color = dd::colors::LimeGreen;
        }

        if (bone.selected) {
            animator->SetEndEffectorBoneIndex(root);
            color = dd::colors::Red;
        }

        if (!skeleton->_bones[root]._children.empty()) {
            // Render child bones from the end of this bone.
            for (std::size_t i = 0; i < skeleton->_bones[root]._children.size(); ++i) {
                RenderSkeletonBone(skeleton, animator, parentTransform, end, skeleton->_bones[root]._children[i], color);
            }
        }
        else {
            // Render leaf node.
            dd::sphere(static_cast<const float*>(&end[0]), color, 0.03f, 0, false);
        }
    }

    void SceneProject3::RenderAnimatedModelPath(Pather *pather) const {
        // Path.
        const float height = pather->GetPathHeight();
        Path &path = pather->GetPath();

        glLineWidth(3.0f);

        ddVec3 orange = {1.0f, 0.5f, 0.0f};
        ddVec3 green = {0.0f, 1.0f, 0.0f};
        ddVec3 white = {1.0f, 1.0f, 1.0f};

        // Control points.
        const std::vector<glm::dvec2> &controlPoints = path.GetControlPoints();
        for (const glm::dvec2 &controlPoint : controlPoints) {
            ddVec3 point{static_cast<float>(controlPoint.x), height, static_cast<float>(controlPoint.y)};
            dd::sphere(point, orange, 0.1f);
        }

        // Interpolated curve.
        if (path.IsValid()) {
            const std::vector<glm::dvec2> &curve = path.GetCurveApproximation();

            for (int i = 0; i < curve.size() - 1; ++i) {
                const glm::dvec2 &start = curve[i];
                const glm::dvec2 &end = curve[i + 1];

                ddVec3 lineStart = {static_cast<float>(start.x), height, static_cast<float>(start.y)};
                ddVec3 lineEnd = {static_cast<float>(end.x), height, static_cast<float>(end.y)};

                dd::line(lineStart, lineEnd, white);
            }
        }
    }

}
