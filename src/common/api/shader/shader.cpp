
#include "common/api/shader/shader.h"

#define INVALID (-1)

namespace Sandbox {

    void Shader::Bind() const {
        glUseProgram(ID_);
    }

    void Shader::Unbind() const {
        glUseProgram(0);
    }

    const std::string &Shader::GetName() const {
        return name_;
    }

    Shader::Shader(const std::string& name, const std::initializer_list<std::string>& shaderComponentPaths) : IReloadable(shaderComponentPaths),
                                                                                                              name_(name),
                                                                                                              ID_(INVALID)
                                                                                                              {
        for (const std::string& filepath : shaderComponentPaths) {
            shaderComponents_.emplace(filepath, ShaderComponent(filepath));
        }

        // Validate extensions.
        for (std::pair<const std::string, ShaderComponent>& data : shaderComponents_) {
            ShaderComponent& component = data.second;
            const ShaderType& type = component.GetType();

            if (type.ToOpenGLType() == GL_INVALID_VALUE) {
                throw std::runtime_error("Unknown or unsupported shader of type: \"" + type.ToString() + "\"");
            }
        }
    }

    Shader::~Shader() {
        glDeleteProgram(ID_);
    }

    void Shader::Recompile() {
        CompileShader();
        Clear();
    }

    void Shader::CompileShader() {
        GLuint shaderProgram = glCreateProgram();
        unsigned numShaderComponents = shaderComponents_.size();
        GLuint* shaders = new GLenum[numShaderComponents];
        unsigned currentShaderIndex = 0;

        //--------------------------------------------------------------------------------------------------------------
        // SHADER COMPONENT COMPILING
        //--------------------------------------------------------------------------------------------------------------
        for (std::pair<const std::string, ShaderComponent>& data : shaderComponents_) {
            ShaderComponent& component = data.second;

            GLuint shader;
            try {
                shader = component.Compile();
            }
            catch (std::runtime_error& e) {
                throw std::runtime_error("Shader '" + name_ + "' failed to compile. " + e.what());
            }

            // Shader successfully compiled.
            glAttachShader(shaderProgram, shader);
            shaders[currentShaderIndex++] = shader;
        }

        //--------------------------------------------------------------------------------------------------------------
        // SHADER PROGRAM LINKING
        //--------------------------------------------------------------------------------------------------------------
        glLinkProgram(shaderProgram);

        GLint isLinked = 0;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
        if (!isLinked) {
            // Shader failed to link - get error information from OpenGL.
            GLint errorMessageLength = 0;
            glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &errorMessageLength);

            std::vector<GLchar> errorMessageBuffer;
            errorMessageBuffer.resize(errorMessageLength + 1);
            glGetProgramInfoLog(shaderProgram, errorMessageLength, nullptr, &errorMessageBuffer[0]);
            std::string errorMessage(errorMessageBuffer.begin(), errorMessageBuffer.end());

            // Program is unnecessary at this point.
            glDeleteProgram(shaderProgram);

            // Delete shader types.
            for (int i = 0; i < numShaderComponents; ++i) {
                glDeleteShader(shaders[i]);
            }

            throw std::runtime_error("Shader: " + name_ + " failed to link. Provided error information: " + errorMessage);
        }

        // Shader has already been initialized, delete prior shader program.
        if (ID_ != INVALID) {
            glDeleteProgram(ID_);
        }
        ID_ = shaderProgram;
        uniformLocations_.clear();

        // Shader types are no longer necessary.
        for (int i = 0; i < numShaderComponents; ++i) {
            GLuint shaderComponentID = shaders[i];
            glDetachShader(shaderProgram, shaderComponentID);
            glDeleteShader(shaderComponentID);
        }
    }

}