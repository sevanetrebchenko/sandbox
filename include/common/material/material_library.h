
#ifndef SANDBOX_MATERIAL_LIBRARY_H
#define SANDBOX_MATERIAL_LIBRARY_H

#include "pch.h"
#include "material.h"
#include "common/api/shader/shader_uniform.h"

namespace Sandbox {

    class MaterialLibrary {
        public:
            MaterialLibrary();
            ~MaterialLibrary();

            void OnImGui();

            void AddMaterial(Material* material);
            void AddMaterial(const std::string& name, std::initializer_list<std::pair<std::string, ShaderUniform::UniformEntry>> uniforms);

            Material* GetMaterial(const std::string& materialName);
            Material* GetMaterialInstance(const std::string& materialName);

        private:
            std::unordered_map<std::string, Material*> _materialList;
    };

}

#endif //SANDBOX_MATERIAL_LIBRARY_H
