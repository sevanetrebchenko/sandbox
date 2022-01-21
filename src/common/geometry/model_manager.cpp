
#include "common/geometry/model_manager.h"
#include "common/utility/imgui_log.h"
#include "common/geometry/object_loader.h"

namespace Sandbox {

    ModelManager::ModelManager() {
    }

    ModelManager::~ModelManager() {
        for (Model* model : _modelList) {
            delete model;
        }
    }

    void ModelManager::OnImGui() {
        if (ImGui::Begin("Scene Hierarchy", nullptr, ImGuiWindowFlags_None)) {
            for (Model* model : _modelList) {
                if (ImGui::TreeNode(model->GetName().c_str())) {
                    DrawModelTransformImGui(model);
                    DrawModelMaterialImGui(model);

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

    Model* ModelManager::AddModelFromFile(std::string modelName, std::string filepath) {
        ImGuiLog& log = ImGuiLog::Instance();

        filepath = NativePathConverter::ConvertToNativeSeparators(filepath);

        // Check file extension.
        std::string extension = filepath.substr(filepath.find_last_of('.') + 1);

        if (extension == "obj") {
            log.LogTrace("Creating new model: %s from .obj file: %s", modelName.c_str(), filepath.c_str());

            // Load in mesh and set up buffers.
            OBJLoader& objLoader = OBJLoader::GetInstance();
            Mesh modelMesh = objLoader.LoadFromFile(filepath);
            modelMesh.Complete();

            log.LogTrace("Finished loading model: %s.", modelName.c_str());

            Model* model = new Model(modelName);
            model->SetMesh(modelMesh);

            _modelList.emplace_back(model);
            return model;
        }
        else {
            log.LogError("Failed to load model file. Loading model files of extension \"%s\" is currently unsupported.", extension.c_str());
            return nullptr;
        }
    }

}
