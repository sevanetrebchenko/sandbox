
#include <scenes/project2/project2.h>
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

    SceneProject2::SceneProject2(int width, int height) : Scene("Motion Along a Path", width, height),
                                                          _fbo(1920, 1080) {
        _dataDirectory = "data/scenes/project2";
    }

    SceneProject2::~SceneProject2() {
    }

    void SceneProject2::OnInit() {
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

    void SceneProject2::OnUpdate(float dt) {
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
    }

    void SceneProject2::OnPreRender() {
    	Backend::Core::ClearColor(glm::vec4(20.0f, 30.0f, 80.0f, 255.0f) / glm::vec4(255.0f));
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SceneProject2::OnRender() {
        ShaderLibrary &shaderLibrary = ShaderLibrary::GetInstance();

        _fbo.BindForReadWrite();
        Backend::Core::SetViewport(0, 0, _fbo.GetWidth(), _fbo.GetHeight());

        _fbo.DrawBuffers();
        Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Shader *phongShader = shaderLibrary.GetShader("Phong");
        phongShader->Bind();

        // Set camera uniforms.
        phongShader->SetUniform("cameraTransform", _camera.GetMatrix());
        phongShader->SetUniform("cameraPosition", _camera.GetEyePosition());
        phongShader->SetUniform("viewTransform", _camera.GetViewMatrix());
        phongShader->SetUniform("cameraNearPlane", _camera.GetNearPlaneDistance());
        phongShader->SetUniform("cameraFarPlane", _camera.GetFarPlaneDistance());

        for (Model *model : _modelManager.GetModels()) {
            Transform &transform = model->GetTransform();
            const Mesh *mesh = model->GetMesh();
            Material *material = model->GetMaterial("Phong");

            // Pre render stage.
            if (material) {
                const glm::mat4 &modelTransform = transform.GetMatrix();
                phongShader->SetUniform("modelTransform", modelTransform);
                phongShader->SetUniform("normalTransform", glm::inverse(modelTransform));

                // Bind all related uniforms with this shader.
                material->Bind(phongShader);
            }

            if (AnimatedModel *animatedModel = dynamic_cast<AnimatedModel *>(model); animatedModel) {
                // Bind animated model uniforms.
                Animator *animator = animatedModel->GetAnimator();
                const std::vector<VQS> &boneTransforms = animator->GetFinalBoneTransformations();
                int numBones = boneTransforms.size();

                phongShader->SetUniform("numBones", numBones);

                for (int i = 0; i < numBones; ++i) {
                    const VQS &vqs = boneTransforms[i];
                    phongShader->SetUniform("finalBoneTransformations[" + std::to_string(i) + "].translation",
                                            vqs.GetTranslation());
                    phongShader->SetUniform("finalBoneTransformations[" + std::to_string(i) + "].rotation",
                                            vqs.GetOrientation().ToVec4());
                    phongShader->SetUniform("finalBoneTransformations[" + std::to_string(i) + "].scale",
                                            vqs.GetScalingFactor());
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

    void SceneProject2::OnPostRender() {
        // Restore viewport.
    }

    void SceneProject2::OnImGui() {
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

    void SceneProject2::OnShutdown() {
        dd::shutdown();
    }

    void SceneProject2::InitializeShaders() {
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

    void SceneProject2::InitializeTextures() {
        TextureLibrary &textureLibrary = TextureLibrary::GetInstance();
    }

    void SceneProject2::InitializeMaterials() {
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

    void SceneProject2::ConfigureModels() {
        MaterialLibrary &materialLibrary = MaterialLibrary::GetInstance();

        AnimatedModel *walkingMan = dynamic_cast<AnimatedModel *>(_modelManager.AddModelFromFile("walking man",
                                                                                                 "assets/models/CesiumMan.glb"));
        Material *material = materialLibrary.GetMaterialInstance("Phong");
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

    void SceneProject2::ConstructFBO() {
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

    void SceneProject2::ConfigureLights() {
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

    void SceneProject2::RenderSkeletonBones(AnimatedModel *animatedModel) const {
        Skeleton *skeleton = animatedModel->GetSkeleton();
        Animator *animator = animatedModel->GetAnimator();

        glLineWidth(3.0f);

        if (skeleton->_drawSkeleton) {
            for (int root : skeleton->_roots) {
                RenderSkeletonBone(skeleton, animator, animatedModel->GetTransform().GetMatrix(),
                                   animatedModel->GetTransform().GetPosition(), root);
            }
        }
    }

    void SceneProject2::RenderSkeletonBone(Skeleton *skeleton, Animator *animator, const glm::mat4 &parentTransform,
                                           const glm::vec3 &origin, int root) const {
        const std::vector<VQS> &finalTransformations = animator->GetFinalBoneTransformations();

        glm::vec3 start = origin;
        glm::vec3 end = glm::vec3(parentTransform * glm::vec4(
                finalTransformations[root] * skeleton->_bones[root]._modelToBoneVQS.GetTranslation(), 1.0f));
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
            dd::line(static_cast<const float *>(&start[0]), static_cast<const float *>(&end[0]), dd::colors::Orange, 0,
                     false);
            dd::cone(static_cast<const float *>(&end[0]), static_cast<const float *>(&direction[0]), dd::colors::Orange,
                     0.025f, 0.0f, 0, false);
        }

        // Render child bones from the end of this bone.
        for (std::size_t i = 0; i < skeleton->_bones[root]._children.size(); ++i) {
            RenderSkeletonBone(skeleton, animator, parentTransform, end, skeleton->_bones[root]._children[i]);
        }
    }

    void SceneProject2::RenderAnimatedModelPath(Pather* pather) const {
        // Path.
        const float height = pather->GetPathHeight();
        Path& path = pather->GetPath();

        glLineWidth(3.0f);

        ddVec3 orange = {1.0f, 0.5f, 0.0f};
        ddVec3 green = {0.0f, 1.0f, 0.0f};
        ddVec3 white = {1.0f, 1.0f, 1.0f};

        // Control points.
        const std::vector<glm::dvec2>& controlPoints = path.GetControlPoints();
        for (const glm::dvec2& controlPoint : controlPoints) {
            ddVec3 point { static_cast<float>(controlPoint.x), height, static_cast<float>(controlPoint.y) };
            dd::sphere(point, orange, 0.1f);
        }

        // Interpolated curve.
        if (path.IsValid()) {
            const std::vector<glm::dvec2>& curve = path.GetCurveApproximation();

            for (int i = 0; i < curve.size() - 1; ++i) {
                const glm::dvec2& start = curve[i];
                const glm::dvec2& end = curve[i + 1];

                ddVec3 lineStart = { static_cast<float>(start.x), height, static_cast<float>(start.y) };
                ddVec3 lineEnd = { static_cast<float>(end.x), height, static_cast<float>(end.y) };

                dd::line(lineStart, lineEnd, white);
            }

            // Draw sphere for POI.
            glm::vec3 poi = pather->GetCurrentPointOfInterest();

            ddVec3 point { static_cast<float>(poi.x), height, static_cast<float>(poi.z) };
            dd::sphere(point, green, 0.1f);
        }
    }

}
