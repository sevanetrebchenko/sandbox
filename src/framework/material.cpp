
#include <framework/material.h>

namespace Sandbox {

    Material::Material(std::string name, Shader* shaderProgram, std::initializer_list<std::pair<std::string, ShaderUniform::UniformEntry>> uniforms) : _name(std::move(name)),
                                                                                                                                         _shaderProgram(shaderProgram) {
        for (const std::pair<std::string, ShaderUniform::UniformEntry>& uniformData : uniforms) {
            const std::string& uniformName = uniformData.first;
            const ShaderUniform::UniformEntry& uniform = uniformData.second;

            _uniforms.emplace(uniformName, new ShaderUniform(uniformName, uniform));
        }
    }

    Material::Material(std::string name) : _name(std::move(name)),
                                           _shaderProgram(nullptr) {
    }

    Material::~Material() {

    }

    Material &Material::operator=(const Material &material) {
        if (&material == this) {
            return *this;
        }

        for (const std::pair<std::string, ShaderUniform*>& uniformData : material._uniforms) {
            _uniforms.emplace(uniformData);
        }

        _shaderProgram = material._shaderProgram;
        return *this;
    }

    void Material::Clear() {
        _uniforms.clear();
    }

    void Material::OnImGui() {
        for (const std::pair<std::string, ShaderUniform*>& uniformData : _uniforms) {
            uniformData.second->OnImGui();
        }
    }

    void Material::Bind(Shader* shaderProgram) const {
        // Only bind uniforms that are tied to this shader or if this shader is valid.
        if (shaderProgram != _shaderProgram || !_shaderProgram) {
            return;
        }

        for (auto& uniformData : _uniforms) {
            uniformData.second->Bind(shaderProgram);
        }
    }

    void Material::Unbind() const {
        for (auto& uniformData : _uniforms) {
            uniformData.second->Unbind();
        }
    }

    void Material::SetShader(Shader *shaderProgram) {
        _shaderProgram = shaderProgram;
    }

    Shader *Material::GetShader() const {
        return _shaderProgram;
    }

    const std::string &Material::GetName() const {
        return _name;
    }

    Material::Material(const Material &other) {
        _name = other._name;
        _shaderProgram = other._shaderProgram;

        for (const auto& uniformData : other._uniforms) {
            const std::string& uniformName = uniformData.first;
            ShaderUniform* uniform = uniformData.second;

            _uniforms.emplace(uniformName, new ShaderUniform(*uniform)); // Deep copy uniform data.
        }
    }

    void Material::SetUniform(const std::string &uniformName, ShaderUniform::UniformEntry uniformData) {
        _uniforms[uniformName] = new ShaderUniform(uniformName, uniformData);
    }

}