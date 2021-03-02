
#ifndef SANDBOX_MATERIAL_LIBRARY_H
#define SANDBOX_MATERIAL_LIBRARY_H

#include <sandbox_pch.h>
#include <framework/material.h>
#include <framework/shader_uniform.h>

namespace Sandbox {

    class MaterialLibrary {
        public:
            static MaterialLibrary& GetInstance();

            void OnImGui();

            void AddMaterial(Material* material);
            void AddMaterial(const std::string& name, Shader* shaderProgram, std::initializer_list<std::pair<std::string, ShaderUniform::UniformEntry>> uniforms);

            Material* GetMaterial(const std::string& materialName);
            Material* GetMaterialInstance(const std::string& materialName);

        private:
            MaterialLibrary();
            ~MaterialLibrary();

            std::unordered_map<std::string, Material*> _materialList;
    };

}

#endif //SANDBOX_MATERIAL_LIBRARY_H
