
#ifndef SANDBOX_MATERIAL_TPP
#define SANDBOX_MATERIAL_TPP

namespace Sandbox {

    template<typename T>
    void Material::SetUniform(const std::string& uniformName, T uniformData) {
        _uniforms[uniformName] = uniformData;
    }

    template<typename T>
    T *Material::GetUniform(const std::string& uniformName) const {
        auto uniformIter = _uniforms.find(uniformName);

        if (uniformIter != _uniforms.end()) {
            return std::get_if<T>(&uniformIter->second); // Nullptr on failure.
        }

        return nullptr;
    }

    template<typename T1, typename T2, typename... T3>
    void Material::BindUniformHelper(const std::pair<std::string, UniformEntry>& uniformEntry) const {
        try {
            T1 value = std::get<T1>(uniformEntry.second);
            _shaderProgram->SetUniform(uniformEntry.first, value);
        }
        catch (std::bad_variant_access& accessException) {
            BindUniformHelper<T2, T3...>(uniformEntry);
        }
    }

    template<typename T>
    void Material::BindUniformHelper(const std::pair<std::string, UniformEntry>& uniformEntry) const {
        try {
            T value = std::get<T>(uniformEntry.second);
            _shaderProgram->SetUniform(uniformEntry.first, value);
        }
        catch (std::bad_variant_access& accessException) {
            throw std::runtime_error("Uniform data type is unsupported.");
        }
    }

}

#endif //SANDBOX_MATERIAL_TPP