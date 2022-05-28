
#include "common/api/shader/shader_component.h"
#include "common/api/shader/shader_preprocessor.h"

namespace Sandbox {

    ShaderComponent::ShaderComponent(const std::string& filepath) : filepath_(filepath) {
    }

    GLuint Sandbox::ShaderComponent::Compile() const {
        ShaderPreprocessor::ShaderInfo info = ShaderPreprocessor::Instance().ProcessFile(filepath_);

        const GLchar* shaderSource = reinterpret_cast<const GLchar*>(info.source.c_str());

        // Create shader from source.
        GLenum type;

        switch (info.type) {
            case ShaderType::VERTEX:
                type = GL_VERTEX_SHADER;
                break;
            case ShaderType::FRAGMENT:
                type = GL_FRAGMENT_SHADER;
                break;
            case ShaderType::GEOMETRY:
                type = GL_GEOMETRY_SHADER;
                break;
            case ShaderType::TESSELATION_CONTROL:
                type = GL_TESS_CONTROL_SHADER; // TODO:
                break;
            case ShaderType::TESSELATION_EVALUATION:
                type = GL_TESS_EVALUATION_SHADER;
                break;
            case ShaderType::COMPUTE:
                type = GL_COMPUTE_SHADER;
                break;
            default:
                type = GL_INVALID_ENUM;
                break;
        }

        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &shaderSource, nullptr); // If length is NULL, each string is assumed to be null terminated.
        glCompileShader(shader);

        // Compile shader source code.
        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (!isCompiled) {
            std::cout << info.filepath << std::endl;
            std::cout << info.source << std::endl;

            // Shader failed to compile - get error information from OpenGL.
            GLint errorMessageLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorMessageLength);

            std::vector<GLchar> errorMessageBuffer;
            errorMessageBuffer.resize(errorMessageLength + 1);
            glGetShaderInfoLog(shader, errorMessageLength, nullptr, &errorMessageBuffer[0]);
            std::string errorMessage(errorMessageBuffer.begin(), errorMessageBuffer.end());

            glDeleteShader(shader);

            std::cerr << errorMessage << std::endl;
            throw std::runtime_error("Failed to compile shader."); // TODO;
        }

        return shader;
    }

}



