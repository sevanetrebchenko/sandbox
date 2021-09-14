
#include <framework/material_library.h>

namespace Sandbox {

    void MaterialLibrary::Initialize() {

    }

    void MaterialLibrary::Shutdown() {
        Clear();
    }

    void MaterialLibrary::OnImGui() {
        if (ImGui::Begin("Material Outliner", nullptr, ImGuiWindowFlags_None)) {
            for (const std::pair<const std::string, Material*>& materialData : materialList_) {
                if (ImGui::TreeNode(materialData.first.c_str())) {
                    materialData.second->OnImGui();

                    ImGui::TreePop();
                }
            }
        }

        ImGui::End();
    }

    void MaterialLibrary::AddMaterial(Material *material) {
        materialList_.emplace(material->GetName(), material);
    }

    void MaterialLibrary::AddMaterial(const std::string& name, std::initializer_list<std::pair<std::string, ShaderUniform::UniformEntry>> uniforms) {
        materialList_.emplace(name, new Material(name, uniforms));
    }

    Material *MaterialLibrary::GetMaterial(const std::string &materialName) {
        auto materialIter = materialList_.find(materialName);

        if (materialIter != materialList_.end()) {
            return materialIter->second;
        }

        return nullptr;
    }

    MaterialLibrary::MaterialLibrary() {
    }

    MaterialLibrary::~MaterialLibrary() {
    }

    Material *MaterialLibrary::GetMaterialInstance(const std::string &materialName) {
        auto materialIter = materialList_.find(materialName);

        if (materialIter != materialList_.end()) {
            return new Material(*materialIter->second);
        }

        return nullptr;
    }

    void MaterialLibrary::Clear() {
        // Clear all materials.
        for (std::pair<const std::string, Material*>& material : materialList_) {
            delete material.second;
        }

        materialList_.clear();
    }

}
