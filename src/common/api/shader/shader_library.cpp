
#include "common/api/shader/shader_library.h"

namespace Sandbox {

    ShaderLibrary& ShaderLibrary::Instance() {
        static ShaderLibrary instance;
        return instance;
    }

    ShaderLibrary::ShaderLibrary() {
    }

    ShaderLibrary::~ShaderLibrary() {
        for (const std::pair<const std::string, Shader*>& data : shaders_) {
            delete data.second;
        }
    }

    Shader* ShaderLibrary::CreateShader(const std::string& name, const std::initializer_list<std::string>& shaderComponentPaths) {
        auto iterator = shaders_.find(name);

        if (iterator != shaders_.end()) {
            throw std::runtime_error("From ShaderLibrary::CreateShader: Instance of shader with the name '" + name + "' already exists.");
        }

        Shader* shader = new Shader(name, shaderComponentPaths);
        shaders_.emplace(name, shader);

        return shader;
    }

    void ShaderLibrary::DestroyShader(const std::string& name) {
        auto iterator = shaders_.find(name);

        if (iterator == shaders_.end()) {
            throw std::runtime_error("From ShaderLibrary::DestroyShader: Instance of shader with the name '" + name + "' does not exist.");
        }

        delete iterator->second;
    }


    Shader *ShaderLibrary::GetShader(const std::string& name) const {
        auto iterator = shaders_.find(name);

        if (iterator != shaders_.end()) {
            return iterator->second;
        }
        return nullptr;
    }

    void ShaderLibrary::RecompileModified() {
        for (const std::pair<const std::string, Shader*>& data : shaders_) {
            Shader* shader = data.second;

            if (shader->IsModified()) {
                shader->Recompile();
            }
        }
    }

    void ShaderLibrary::RecompileAll() {
        for (const std::pair<const std::string, Shader*>& data : shaders_) {
            Shader* shader = data.second;
            shader->Recompile();
        }
    }

}