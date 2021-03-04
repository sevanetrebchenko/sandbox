
#include <framework/material_library.h>

namespace Sandbox {


    MaterialLibrary &MaterialLibrary::GetInstance() {
        static MaterialLibrary materialLibrary;
        return materialLibrary;
    }

    void MaterialLibrary::OnImGui() {
        if (ImGui::Begin("Material Outliner", nullptr, ImGuiWindowFlags_None)) {
            for (const std::pair<std::string, Material*>& materialData : _materialList) {
                if (ImGui::TreeNode(materialData.first.c_str())) {
                    materialData.second->OnImGui();

                    ImGui::TreePop();
                }
            }
        }

        ImGui::End();
    }

    void MaterialLibrary::AddMaterial(Material *material) {
        _materialList.emplace(material->GetName(), material);
    }

    void MaterialLibrary::AddMaterial(const std::string& name, Shader *shaderProgram, std::initializer_list<std::pair<std::string, ShaderUniform::UniformEntry>> uniforms) {
        _materialList.emplace( name, new Material(name, shaderProgram, uniforms));
    }

    Material *MaterialLibrary::GetMaterial(const std::string &materialName) {
        auto materialIter = _materialList.find(materialName);

        if (materialIter != _materialList.end()) {
            return materialIter->second;
        }

        return nullptr;
    }

    MaterialLibrary::MaterialLibrary() {

    }

    MaterialLibrary::~MaterialLibrary() {

    }

    Material *MaterialLibrary::GetMaterialInstance(const std::string &materialName) {
        auto materialIter = _materialList.find(materialName);

        if (materialIter != _materialList.end()) {
            return new Material(*materialIter->second);
        }

        return nullptr;
    }

}
