
#ifndef SANDBOX_MATERIAL_LIBRARY_H
#define SANDBOX_MATERIAL_LIBRARY_H

#include <sandbox.h>
#include <framework/material.h>
#include <framework/shader_uniform.h>
#include <framework/singleton.h>

namespace Sandbox {

    class MaterialLibrary : public Singleton<MaterialLibrary> {
        public:
            REGISTER_SINGLETON(MaterialLibrary);

            void Initialize() override;
            void Shutdown() override;

            void OnImGui();

            void AddMaterial(Material* material);
            void AddMaterial(const std::string& name, std::initializer_list<std::pair<std::string, ShaderUniform::UniformEntry>> uniforms);

            Material* GetMaterial(const std::string& materialName);
            Material* GetMaterialInstance(const std::string& materialName);

            void Clear();

        private:
            MaterialLibrary();
            ~MaterialLibrary() override;

            std::unordered_map<std::string, Material*> materialList_;
    };

}

#endif //SANDBOX_MATERIAL_LIBRARY_H
