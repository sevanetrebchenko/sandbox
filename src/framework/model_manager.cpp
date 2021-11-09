
#include <framework/model_manager.h>
#include <framework/object_loader.h>
#include <framework/assimp_loader.h>
#include <framework/imgui_log.h>
#include <framework/buffer/ubo.h>
#include <framework/ubo_manager.h>

namespace Sandbox {

    ModelManager::ModelManager() = default;

    ModelManager::~ModelManager() {
        for (Model* model : _modelList) {
            delete model;
        }
    }

    void ModelManager::Update(float dt) {
        for (Model* model : _modelList) {
            model->Update(dt);
        }
    }

    void ModelManager::OnImGui() {
        if (ImGui::Begin("Scene Hierarchy", nullptr, ImGuiWindowFlags_None)) {
            for (Model* model : _modelList) {
                if (ImGui::TreeNode(model->GetName().c_str())) {
                    DrawModelTransformImGui(model);
                    DrawModelMaterialImGui(model);
                    DrawModelMeshImgui(model);
                    DrawModelSkeletonImGui(model);
                    DrawModelAnimatorImGui(model);
                    DrawModelPatherImGui(model);

                    ImGui::TreePop();
                }
            }
        }

        ImGui::End();
    }

    Model* ModelManager::GetNamedModel(std::string modelName) const {
        modelName = NativePathConverter::ConvertToNativeSeparators(modelName);

        for (Model* model : _modelList) {
            if (model->GetName() == modelName) {
                return model;
            }
        }

        return nullptr;
    }

    const std::vector<Model*> &ModelManager::GetModels() const {
        return _modelList;
    }

    void ModelManager::DrawModelTransformImGui(Model* model) const {
        if (ImGui::TreeNode("Transform")) {
            model->GetTransform().OnImGui();

            ImGui::TreePop();
        }
    }

    void ModelManager::DrawModelMaterialImGui(Model *model) const {
        if (ImGui::TreeNode("Materials List")) {
            for (Material* material : model->GetMaterialsList()) {

                // Draw individual material components.
                if (ImGui::TreeNode(material->GetName().c_str())) {
                    material->OnImGui();

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }

    void ModelManager::DrawModelMeshImgui(Model *model) const {
        if (ImGui::TreeNode("Mesh")) {
            ImGui::PushStyleColor(ImGuiCol_Text, 0xff999999);
                ImGui::Text("Vertex Count: %zu", model->GetMesh()->GetVertices().size());
                ImGui::Text("Index Count: %zu", model->GetMesh()->GetIndices().size());
            ImGui::PopStyleColor();
            ImGui::TreePop();
        }
    }

    void ModelManager::DrawModelSkeletonImGui(Model *model) const {
        AnimatedModel* animatedModel = dynamic_cast<AnimatedModel*>(model);
        if (!animatedModel) {
            return;
        }

        if (ImGui::TreeNode("Skeleton")) {
            Skeleton* skeleton = animatedModel->GetSkeleton();

            ImGui::Text("Render Skeleton Bones");
            ImGui::Checkbox("##drawSkeleton", &skeleton->_drawSkeleton);

            ImGui::PushStyleColor(ImGuiCol_Text, 0xff999999);
                // Bone count.
                ImGui::Text("Bone Count: %zu", skeleton->_bones.size());

                // Bone hierarchy.
                skeleton->DrawImGui();
            ImGui::PopStyleColor();

            ImGui::TreePop();
        }
    }

    void ModelManager::DrawModelAnimatorImGui(Model *model) const {
        AnimatedModel* animatedModel = dynamic_cast<AnimatedModel*>(model);
        if (!animatedModel) {
            return;
        }

        Animator* animator = animatedModel->GetAnimator();
        if (!animator) {
            return;
        }

        if (ImGui::TreeNode("Animator")) {
            // Animation name.
            ImGui::PushStyleColor(ImGuiCol_Text, 0xff999999);
                ImGui::Text("Current Animation: %s", animator->GetCurrentAnimation()->_name.c_str());
            ImGui::PopStyleColor();

            // Interpolation methods.
            // Key interpolation method.
            {
                std::string selected = ToString(animator->GetKeyInterpolationMethod());
                std::vector<std::string> interpolationMethods = { "Default", "Incremental" };

                ImGui::Text("Key Interpolation Method:");

                if (ImGui::BeginCombo("##keyInterpolationMethods", selected.c_str())) {
                    for (const std::string& method : interpolationMethods) {
                        bool isSelected = (selected == method);
                        if (ImGui::Selectable(method.c_str(), isSelected)) {
                            selected = method;

                            if (selected == "Default") {
                                animator->SetKeyInterpolationMethod(KeyInterpolationMethod::DEFAULT);
                            }
                            else if (selected == "Incremental") {
                                animator->SetKeyInterpolationMethod(KeyInterpolationMethod::INCREMENTAL);
                            }
                        }

                        if (isSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
            }

            // Quaternion interpolation method.
            {
                std::vector<std::string> interpolationMethods = { "Lerp", "Slerp" };
                std::string selected = ToString(animator->GetQuaternionInterpolationMethod());

                ImGui::Text("Quaternion Interpolation Method:");

                if (ImGui::BeginCombo("##quaternionInterpolationMethods", selected.c_str())) {
                    for (const std::string& method : interpolationMethods) {
                        bool isSelected = (selected == method);
                        if (ImGui::Selectable(method.c_str(), isSelected)) {
                            selected = method;

                            if (selected == "Lerp") {
                                animator->SetQuaternionInterpolationMethod(QuaternionInterpolationMethod::LERP);
                            }
                            else if (selected == "Slerp") {
                                animator->SetQuaternionInterpolationMethod(QuaternionInterpolationMethod::SLERP);
                            }
                        }

                        if (isSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
            }

            // Bind pose toggle.
            bool isBindPose = animator->IsBindPoseActive();
            ImGui::Text("Show Bind Pose");
            if (ImGui::Checkbox("##bindPose", &isBindPose)) {
                animator->SetBindPoseActive(isBindPose);
            }

            // Animation playback speed.
            ImGui::Text("Animation Playback Speed:");
            float playbackSpeed = animator->GetPlaybackSpeed();
            if (ImGui::DragFloat("##playbackSpeed", &playbackSpeed, 0.05f, 0.0f, 2.0f)) {
                animator->SetPlaybackSpeed(playbackSpeed);
            }

            ImGui::TreePop();
        }
    }

    void ModelManager::DrawModelPatherImGui(Model *model) const {
        AnimatedModel* animatedModel = dynamic_cast<AnimatedModel*>(model);
        if (!animatedModel) {
            return;
        }

        Pather* pather = animatedModel->GetPather();
        if (!pather) {
            return;
        }

        Path& path = pather->GetPath();

        if (ImGui::TreeNode("Pather")) {
            // Animation name.
            ImGui::PushStyleColor(ImGuiCol_Text, 0xff999999);
            ImGui::PopStyleColor();

            float cycleTime = pather->GetCompletionTime();
            ImGui::Text("Path Cycle Time (seconds):");
            if (ImGui::DragFloat("##cycleTime", &cycleTime, 0.05f, 0.01f, 100.0f)) {
            	pather->SetCompletionTime(cycleTime);
            }

            ImGui::Separator();

            if (ImGui::Button("Clear")) {
                path.Clear();
            }

            ImGui::SameLine();

            if (ImGui::Button("Recompute")) {
                path.Recompute();
            }

            if (ImPlot::BeginPlot("##pathOutliner", ImVec2(-1, 0), ImPlotFlags_CanvasOnly)) {
                // Axes.
                ImPlot::SetupAxesLimits(-15, 15, -15, 15);
                ImPlot::SetupAxis(ImAxis_Y1, "Z", ImPlotAxisFlags_Invert);
                ImPlot::SetupAxis(ImAxis_X1, "X", ImPlotAxisFlags_None);

                if (ImGui::IsItemHovered()) {
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                        ImPlotPoint newPointPosition = ImPlot::GetPlotMousePos();

                        // Emplace new point and recompute new curve.
                        glm::dvec2 point(newPointPosition.x, newPointPosition.y);
                        path.AddControlPoint(point);

                        path.Recompute();
                    }
                }

                // Visualize control points.
                std::vector<glm::dvec2> controlPoints = path.GetControlPoints();
                bool invalidated = false;

                for (int i = 0; i < controlPoints.size(); ++i) {
                    glm::dvec2 &point = controlPoints[i];
                    if (ImPlot::DragPoint(i, &point.x, &point.y, ImVec4(1, 1, 1, 1))) {
                        invalidated = true;
                    }
                }

                if (invalidated) {
                    path.SetControlPoints(controlPoints);
                    path.Recompute();
                }

                // Draw curve.
                if (path.IsValid()) {
                    const std::vector<glm::dvec2>& curve = path.GetCurveApproximation();

                    std::vector<double> curveXCoordinates;
                    std::vector<double> curveYCoordinates;

                    for (const glm::dvec2& point : curve) {
                        curveXCoordinates.push_back(point.x);
                        curveYCoordinates.push_back(point.y);
                    }

                    ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1, 1, 1, 1));
                    ImPlot::PlotLine("Path", curveXCoordinates.data(), curveYCoordinates.data(), curve.size());
                }

                ImPlot::EndPlot();
            }

            ImGui::TreePop();
        }
    }

    Model* ModelManager::AddModelFromFile(std::string modelName, std::string filepath) {
        ImGuiLog& log = ImGuiLog::GetInstance();

        filepath = NativePathConverter::ConvertToNativeSeparators(filepath);

        // Check file extension.
        std::string extension = filepath.substr(filepath.find_last_of('.') + 1);

        if (extension == "obj") {
            log.LogTrace("Creating new model: '%s' from .obj file: %s", modelName.c_str(), filepath.c_str());

            // Load in mesh and set up buffers.
            OBJLoader& objLoader = OBJLoader::GetInstance();
            Mesh modelMesh = objLoader.LoadFromFile(filepath);
            modelMesh.Complete();

            log.LogTrace("Finished loading model: '%s'.", modelName.c_str());

            Model* model = new Model(modelName);
            model->SetMesh(new Mesh(modelMesh));

            _modelList.emplace_back(model);
            return model;
        }
        else if (extension == "glb") {
            log.LogTrace("Creating new model: '%s' from .glb file: %s", modelName.c_str(), filepath.c_str());

            // Load in mesh and set up buffers.
            AssimpLoader& objLoader = AssimpLoader::GetInstance();
            AnimatedModel* model = objLoader.LoadFromFile(filepath);

            log.LogTrace("Finished loading model: '%s'.", modelName.c_str());

            _modelList.emplace_back(model);
            return model;
        }

        else {
            log.LogError("Failed to load model file. Loading model files of extension \"%s\" is currently unsupported.", extension.c_str());
            return nullptr;
        }
    }

}
