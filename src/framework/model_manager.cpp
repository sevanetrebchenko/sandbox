
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

        if (ImGui::TreeNode("Animator")) {
            Animator* animator = animatedModel->GetAnimator();

            // Animation name.
            ImGui::PushStyleColor(ImGuiCol_Text, 0xff999999);
                ImGui::Text("Current Animation: %s", animator->GetCurrentAnimation()->_name.c_str());
            ImGui::PopStyleColor();

            // Bind pose toggle.
            bool isBindPose = animator->IsBindPoseActive();
            if (ImGui::Checkbox("Bind Pose: ", &isBindPose)) {
                animator->SetBindPoseActive(isBindPose);
            }

            ImGui::Text("Animation Playback Speed:");
            float playbackSpeed = animator->GetPlaybackSpeed();
            if (ImGui::DragFloat("##playbackSpeed", &playbackSpeed, 0.05f, 0.0f, 2.0f)) {
                animator->SetPlaybackSpeed(playbackSpeed);
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
