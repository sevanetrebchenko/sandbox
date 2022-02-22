
#ifndef SANDBOX_SHADER_H
#define SANDBOX_SHADER_H

#include "pch.h"
#include "common/utility/reloadable.h"
#include "common/api/shader/shader_component.h"

namespace Sandbox {

    class Shader : public IReloadable {
        public:
            void Bind() const;
            void Unbind() const;

            [[nodiscard]] const std::string& GetName() const;

            template <typename DataType>
            void SetUniform(const std::string& uniformName, DataType value);

        private:
            friend class ShaderLibrary;
            Shader(const std::string& name, const std::initializer_list<std::string>& shaderComponentPaths);
            ~Shader();

            void Recompile() override;

            void CompileShader();

            template<typename DataType>
            void SetUniformData(GLuint uniformLocation, DataType value);

            std::string name_;
            GLuint ID_;

            std::unordered_map<std::string, ShaderComponent> shaderComponents_;
            std::unordered_map<std::string, GLint> uniformLocations_;
    };

}

#include "common/api/shader/shader.tpp"

#endif //SANDBOX_SHADER_H
