
#include "common/material/material_library.h"

namespace Sandbox {

    MaterialLibrary::MaterialLibrary() {

    }

    MaterialLibrary::~MaterialLibrary() {

    }

    void MaterialLibrary::OnImGui() {
        if (ImGui::Begin("Material Outliner", nullptr, ImGuiWindowFlags_None)) {
            for (const std::pair<const std::string, Material*>& materialData : _materialList) {
                const std::string& name = materialData.first;
                Material* material = materialData.second;

                if (ImGui::TreeNode(name.c_str())) {
                    material->OnImGui();

                    ImGui::TreePop();
                }
            }
        }

        ImGui::End();
    }

    void MaterialLibrary::AddMaterial(Material *material) {
        _materialList.emplace(material->GetName(), material);
    }

    void MaterialLibrary::AddMaterial(const std::string& name, std::initializer_list<std::pair<std::string, ShaderUniform::UniformEntry>> uniforms) {
        _materialList.emplace(name, new Material(name, uniforms));
    }

    Material *MaterialLibrary::GetMaterial(const std::string &materialName) {
        auto materialIter = _materialList.find(materialName);

        if (materialIter != _materialList.end()) {
            return materialIter->second;
        }

        return nullptr;
    }

    Material *MaterialLibrary::GetMaterialInstance(const std::string &materialName) {
        auto materialIter = _materialList.find(materialName);

        if (materialIter != _materialList.end()) {
            return new Material(*materialIter->second);
        }

        return nullptr;
    }

}
