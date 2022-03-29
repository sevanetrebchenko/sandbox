
#include "common/api/shader/shader.h"
#include "common/utility/log.h"
#include "common/application/application.h"
#include "common/api/shader/shader_preprocessor.h"

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
        Recompile();
    }

    Shader::~Shader() {
        glDeleteProgram(ID_);
    }

    void Shader::Recompile() {
        UpdateEditTimes();
        CompileFromSource();
//
//        std::string path = GetCachedBinaryPath();
//        if (Exists(path)) {
//            // Shader should be loaded directly from the shader cache if the last edit time on ALL the individual shader components
//            // is before the last edit time of the shader binary code (shader source has not been edited, and shader binary still
//            // holds the most up-to-date versions of the components).
//            bool binaryOutdated = false;
//            std::filesystem::file_time_type binaryEditTime = std::filesystem::last_write_time(GetCachedBinaryPath());
//
//            for (const std::filesystem::file_time_type& componentEditTime : GetEditTimes()) {
//                if (componentEditTime > binaryEditTime) {
//                    binaryOutdated = true;
//                    break;
//                }
//            }
//
//            if (binaryOutdated) {
//                CompileFromSource();
//            }
//            else {
//                CompileFromSource();
////                CompileFromBinary(); // TODO.
//            }
//        }
//        else {
//            // Binary file has not been created yet.
//            CompileFromSource();
//        }
    }


    void Shader::CompileFromSource() {
        ImGuiLog::Instance().LogTrace("Compiling Shader '%s' from source.", name_.c_str());

        GLuint shaderProgram = glCreateProgram();
        unsigned numShaderComponents = shaderComponents_.size();
        GLuint shaders[numShaderComponents];
        unsigned currentComponentIndex = 0;

        //--------------------------------------------------------------------------------------------------------------
        // SHADER COMPONENT COMPILING
        //--------------------------------------------------------------------------------------------------------------
        for (std::pair<const std::string, ShaderComponent>& data : shaderComponents_) {
            const std::string& path = data.first;
            ShaderComponent& component = data.second;
            GLuint componentID = component.Compile();

            // Shader successfully compiled.
            glAttachShader(shaderProgram, componentID);
            shaders[currentComponentIndex++] = componentID;
        }

        //--------------------------------------------------------------------------------------------------------------
        // SHADER PROGRAM LINKING
        //--------------------------------------------------------------------------------------------------------------
        glLinkProgram(shaderProgram);

        GLint isLinked = 0;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE) {
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

            ImGuiLog::Instance().LogError("%s", errorMessage.c_str());
            throw std::runtime_error("Shader '" + name_ + "' failed to link. See out/log.txt for details.");
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
    }

}