
#include <framework/material.h>

namespace Sandbox {

    Material::Material(std::string name, std::initializer_list<std::pair<std::string, ShaderUniform::UniformEntry>> uniforms) : _name(std::move(name)) {
        for (const std::pair<std::string, ShaderUniform::UniformEntry>& uniformData : uniforms) {
            const std::string& uniformName = uniformData.first;
            const ShaderUniform::UniformEntry& uniform = uniformData.second;

            _uniforms.emplace(uniformName, new ShaderUniform(uniformName, uniform));
        }
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
        for (auto& uniformData : _uniforms) {
            uniformData.second->Bind(shaderProgram);
        }
    }

    void Material::Unbind() const {
        for (auto &uniformData : _uniforms) {
            uniformData.second->Unbind();
        }
    }

    const std::string &Material::GetName() const {
        return _name;
    }

    Material::Material(const Material &other) {
        _name = other._name;

        for (const auto& uniformData : other._uniforms) {
            const std::string& uniformName = uniformData.first;
            ShaderUniform* uniform = uniformData.second;

            _uniforms.emplace(uniformName, new ShaderUniform(*uniform)); // Deep copy uniform data.
        }
    }

    void Material::SetUniform(const std::string &uniformName, ShaderUniform::UniformEntry uniformData) {
        _uniforms[uniformName] = new ShaderUniform(uniformName, uniformData);
    }

    ShaderUniform *Material::GetUniform(const std::string &uniformName) const {
        auto uniformIter = _uniforms.find(uniformName);
        if (uniformIter != _uniforms.end()) {
            return uniformIter->second;
        }

        return nullptr;
    }

}