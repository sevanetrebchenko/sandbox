
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

        // Validate extensions.
        for (const std::pair<const std::string, ShaderComponent>& data : shaderComponents_) {
            const std::string& componentPath = data.first;
            const ShaderComponent& component = data.second;
            const ShaderType& type = component.GetType();

            if (type.ToOpenGLType() == GL_INVALID_VALUE) {
                ImGuiLog::Instance().LogError("Encountered unknown / unsupported shader of type '%s' in ShaderComponent '%s'.", type.ToString().c_str(), componentPath.c_str());
                throw std::runtime_error("Encountered unknown / unsupported shader of type. See out/log.txt for details.");
            }
        }

        Recompile();
    }

    Shader::~Shader() {
        glDeleteProgram(ID_);
    }

    void Shader::Recompile() {
        UpdateEditTimes();

        std::string path = GetCachedBinaryPath();
        if (Exists(path)) {
            // Shader should be loaded directly from the shader cache if the last edit time on ALL the individual shader components
            // is before the last edit time of the shader binary code (shader source has not been edited, and shader binary still
            // holds the most up-to-date versions of the components).
            bool binaryOutdated = false;
            std::filesystem::file_time_type binaryEditTime = std::filesystem::last_write_time(GetCachedBinaryPath());

            for (const std::filesystem::file_time_type& componentEditTime : GetEditTimes()) {
                if (componentEditTime > binaryEditTime) {
                    binaryOutdated = true;
                    break;
                }
            }

            if (binaryOutdated) {
                CompileFromSource();
            }
            else {
                CompileFromSource();
//                CompileFromBinary();
            }
        }
        else {
            // Binary file has not been created yet.
            CompileFromSource();
        }
    }

    void Shader::CacheShaderBinary() {
        GLint numFormats = 0;
        glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numFormats);
        if (numFormats < 1) {
            ImGuiLog::Instance().LogError("Function Shader::CacheShaderBinary called, but hardware does not support shader binary formats.");
            throw std::runtime_error("Failed to cache binary shader code for shader '" + name_ + "'. See out/log.txt for details.");
        }

        GLint length = 0;
        glGetProgramiv(ID_, GL_PROGRAM_BINARY_LENGTH, &length);

        // Retrieve shader program binary.
        std::vector<GLubyte> buffer(length);
        GLenum format = 0;
        glGetProgramBinary(ID_, length, nullptr, &format, buffer.data());

        // Write the binary to a file.
        std::string path = GetCachedBinaryPath();

        std::ofstream writer(path.c_str(), std::ios::out | std::ios::binary);
        if (writer.is_open()) {
            writer.write(reinterpret_cast<char*>(buffer.data()), length);
            writer.flush();
            writer.close();
        }
        else {
            ImGuiLog::Instance().LogError("Failed to open file '%s' for shader binary.", path.c_str());
            throw std::runtime_error("Failed to cache binary shader code for shader '" + name_ + "'. See out/log.txt for details.");
        }
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
            ShaderComponent::CompilationResult result = component.Compile();

            if (result.success) {
                GLuint componentID = result.ID;

                if (result.numWarnings > 0) {
                    ImGuiLog::Instance().LogWarning("ShaderComponent '%s' of Shader '%s' successfully compiled with %i warnings.", path.c_str(), name_.c_str(), result.numWarnings);
                }
                else {
                    ImGuiLog::Instance().LogWarning("ShaderComponent '%s' of Shader '%s' successfully compiled.", path.c_str(), name_.c_str());
                }

                // Shader successfully compiled.
                glAttachShader(shaderProgram, componentID);
                shaders[currentComponentIndex++] = componentID;
            }
            else {
                ImGuiLog::Instance().LogError("ShaderComponent '%s' of Shader '%s' compiled with %i warnings and %i errors.", name_.c_str(), path.c_str(), result.numWarnings, result.numErrors);
                throw std::runtime_error("Shader '" + name_ + "' failed to compile. See out/log.txt for details.");
            }
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

        // Shader has been updated, update binary cache.
        CacheShaderBinary();
    }

    void Shader::CompileFromBinary() {
//        ImGuiLog::Instance().LogTrace("Compiling Shader '%s' from binary.", name_.c_str());
//        GLuint shaderProgram = glCreateProgram();
//
//        // Read shader binary.
//        std::string path = GetCachedBinaryPath();
//        std::ifstream reader(path, std::ios::binary);
//
//        if (!reader.is_open()) {
//            ImGuiLog::Instance().LogError("Failed to open binary file of Shader '%s' (%s).", name_.c_str(), path.c_str());
//            throw std::runtime_error("Failed to compile Shader '" + name_ + "' from binary. See out/log.txt for details.");
//        }
//
//        std::vector<char> buffer(std::istreambuf_iterator<char>(reader), { });
//        reader.close();
//
//        // Construct shader program.
//        GLint numFormats = 0;
//        glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numFormats);
//        GLenum binaryFormats[numFormats];
//        glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, binaryFormats);
//
//        GLint format;
//        glProgramBinary(shaderProgram, binaryFormats, buffer.data(), buffer.size() );
//
//        GLint isLinked = 0;
//        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
//        if (!isLinked) {
//            // Failed to link shader binary.
//            ImGuiLog::Instance().LogError("Failed to construct Shader '%s' from binary, defaulting back to full recompilation from source.", name_.c_str());
//            CompileFromSource(); // Shader ID gets set, shader binary gets re-cached.
//        }
//        else {
//            if (ID_ != INVALID) {
//                glDeleteProgram(ID_);
//                uniformLocations_.clear();
//            }
//            ID_ = shaderProgram;
//        }
    }

    std::string Shader::GetCachedBinaryPath() const {
        const std::string& directory = Application::Instance().GetSceneManager().GetActiveScene()->GetShaderCacheDirectory();
        std::string name = ProcessName(name_);
        return ConvertToNativeSeparators(directory + "/" + name + ".bin");
    }

    void Shader::CompileToSPIRV() {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        // No difference between OpenGL 4.5 and 4.6 (from documentation).
        options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
        options.SetSourceLanguage(shaderc_source_language_glsl);
        options.SetForcedVersionProfile(, shaderc_profile_core); // TODO;
        options.SetOptimizationLevel(shaderc_optimization_level_performance);

        using word = std::uint32_t;
        std::unordered_map<std::string, std::vector<word>> componentBinary;
        std::vector<std::vector<std::uint32_t>> binary { };

        for (const std::pair<const std::string, ShaderComponent>& data : shaderComponents_) {
            const std::string& path = data.first;
            const ShaderComponent& component = data.second;
            shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(component.GetSource(), component.GetType().ToSPIRVType(), path.c_str(), "main", options);

            if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
                // TODO.
            }

            binary.emplace_back(module.cbegin(), module.cend());
        }

        return {module.cbegin(), module.cend()};
    }

}