
#ifndef SANDBOX_SHADER_H
#define SANDBOX_SHADER_H

#include <sandbox.h>
#include <framework/directory_utils.h>

namespace Sandbox {

    class Shader : public IReloadable {
        public:
            Shader(std::string name, const std::initializer_list<std::string>& shaderComponentPaths);
            ~Shader();

            void Bind() const;
            void Unbind() const;

            void Recompile();

            [[nodiscard]] const std::string& GetName() const;

            template <typename DataType>
            void SetUniform(const std::string& uniformName, DataType value);

        private:
            void OnFileModified() override;

            std::string ReadFile(const std::string& filePath);
            std::unordered_map<std::string, std::pair<GLenum, std::string>> GetShaderSources();
            void CompileShader(const std::unordered_map<std::string, std::pair<GLenum, std::string>>& shaderComponents);
            std::string ShaderTypeToString(GLenum shaderType) const;
            GLenum ShaderTypeFromString(const std::string& shaderExtension);
            GLuint CompileShaderComponent(const std::pair<std::string, std::pair<GLenum, std::string>>& shaderComponent);

            template<typename DataType>
            void SetUniformData(GLuint uniformLocation, DataType value);

            std::vector<std::string> _shaderComponentPaths;
            std::unordered_map<std::string, GLint> _uniformLocations;
            std::string _shaderName;
            GLuint _shaderID;
    };

}

#include <framework/shader.tpp>

#endif //SANDBOX_SHADER_H
