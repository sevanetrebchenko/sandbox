
#include "common/api/shader/shader.h"
#include "common/utility/log.h"
#include "common/application/application.h"

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

        Recompile();
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
        GLuint shaders[numShaderComponents];
        unsigned currentComponentIndex = 0;

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
            shaders[currentComponentIndex++] = shader;
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

        // Shader has already been initialized, do cleanup first.
        if (ID_ != INVALID) {
            glDeleteProgram(ID_);
            uniformLocations_.clear();
        }
        ID_ = shaderProgram;

        // Shader types are no longer necessary.
        for (int i = 0; i < numShaderComponents; ++i) {
            GLuint shaderComponentID = shaders[i];
            glDetachShader(shaderProgram, shaderComponentID);
            glDeleteShader(shaderComponentID);
        }

        CacheShaderBinary();
    }

    void Shader::CacheShaderBinary() {
        GLint numFormats = 0;
        glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numFormats);

        if (numFormats < 1) {
            LogWarningOnce("Driver does not support shader binary formats. Shaders will always be recompiled on program startup.");
        }
        else {
            GLint length = 0;
            glGetProgramiv(ID_, GL_PROGRAM_BINARY_LENGTH, &length);

            // Retrieve shader program binary.
            std::vector<GLubyte> buffer(length);
            GLenum format = 0;
            glGetProgramBinary(ID_, length, nullptr, &format, buffer.data());

            // Write the binary to a file.
            const std::string& directory = Application::Instance().GetSceneManager().GetActiveScene()->GetShaderCacheDirectory();
            std::string filepath = ConvertToNativeSeparators(directory + "/" + name_ + ".bin");

            std::ofstream writer(filepath.c_str(), std::ios::out | std::ios::binary);
            if (writer.is_open()) {
                writer.write(reinterpret_cast<char*>(buffer.data()), length);
                writer.flush();
                writer.close();
            }
        }
    }

}