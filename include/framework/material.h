
#ifndef SANDBOX_MATERIAL_H
#define SANDBOX_MATERIAL_H

#include <sandbox_pch.h>
#include <framework/shader.h>
#include <framework/shader_uniform.h>

namespace Sandbox {

    class Material {
        public:
            Material(std::string name, Shader* shaderProgram, std::initializer_list<std::pair<std::string, ShaderUniform::UniformEntry>> uniforms);
            explicit Material(std::string name);
            ~Material();

            Material(const Material& other);
            Material& operator=(const Material& material);

            void OnImGui();

            void Bind(Shader* shaderProgram) const;
            void Unbind() const;

            void Clear();

            void SetShader(Shader* shaderProgram);
            Shader* GetShader() const;

            const std::string& GetName() const;

            void SetUniform(const std::string& uniformName, ShaderUniform::UniformEntry uniformData);
            ShaderUniform* GetUniform(const std::string& uniformName) const;

        private:
            std::string _name;

            Shader* _shaderProgram;
            std::unordered_map<std::string, ShaderUniform*> _uniforms;
    };

}

#endif //SANDBOX_MATERIAL_H
