
#ifndef SANDBOX_MATERIAL_H
#define SANDBOX_MATERIAL_H

#include "pch.h"
#include "common/api/shader/shader.h"
#include "common/api/shader/shader_uniform.h"
#include "common/ecs/component/component.h"

namespace Sandbox {

    class Material {
        public:
            Material(std::string name, std::initializer_list<std::pair<std::string, ShaderUniform::UniformEntry>> uniforms);
            ~Material();

            Material(const Material& other);
            Material& operator=(const Material& material);

            void OnImGui();

            void Bind(Shader* shaderProgram) const;
            void Unbind() const;

            void Clear();
            const std::string& GetName() const;

            void SetUniform(const std::string& uniformName, ShaderUniform::UniformEntry uniformData);
            ShaderUniform* GetUniform(const std::string& uniformName) const;

        private:
            std::string _name;
            std::unordered_map<std::string, ShaderUniform*> _uniforms;
    };

    class MaterialCollection : public IComponent {
        public:
            MaterialCollection();
            ~MaterialCollection() override;

            void SetMaterial(Material* material);
            [[nodiscard]] Material* GetNamedMaterial(const std::string& name);

        private:
            std::vector<Material*> materials_;
    };

}

#endif //SANDBOX_MATERIAL_H
