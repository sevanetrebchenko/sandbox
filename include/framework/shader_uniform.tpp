
#ifndef SANDBOX_SHADER_UNIFORM_TPP
#define SANDBOX_SHADER_UNIFORM_TPP

#include <sandbox_pch.h>

namespace Sandbox {

    template <typename T>
    void ShaderUniform::SetData(const T &data) {
        SetDataHelper<T, SUPPORTED_UNIFORM_TYPES>(data);
    }

    template<typename Type, typename T1, typename... T2>
    void ShaderUniform::SetDataHelper(const Type &data) {
        if constexpr (std::is_same_v<Type, T1>) {
            TrySetData(data);
        }
        else {
            SetDataHelper<Type, T2...>(data);
        }
    }

    template <typename Type, typename T1>
    void ShaderUniform::SetDataHelper(const Type& data) {
        if constexpr (std::is_same_v<Type, T1>) {
            TrySetData(data);
        }
        else {
            throw std::runtime_error("Unknown data type provided into ShaderUniform::SetData.");
        }
    }

    template<typename T>
    void ShaderUniform::TrySetData(const T &data) {
        if (const T* type = std::get_if<T>(&_uniformData)) {
            _uniformData = data;
        }
    }

    template<typename T1, typename T2, typename... T3>
    void ShaderUniform::BindHelper(Shader *shaderProgram) const {
        if (const T1* type = std::get_if<T1>(&_uniformData)) {
            shaderProgram->SetUniform(_uniformName, *type);
        }
        else {
            BindHelper<T2, T3...>(shaderProgram);
        }
    }

    template <typename T>
    void ShaderUniform::BindHelper(Shader* shaderProgram) const {
        if (const T* type = std::get_if<T>(&_uniformData)) {
            shaderProgram->SetUniform(_uniformName, *type);
        }
    }

    // bool, int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4
    template<typename T1, typename T2, typename... T3>
    void ShaderUniform::OnImGuiHelper() {
        if (const T1* type = std::get_if<T1>(&_uniformData)) {
            OnImGuiForType<T1>(*type);
        }
        else {
            OnImGuiHelper<T2, T3...>();
        }
    }

    template <typename T>
    void ShaderUniform::OnImGuiHelper() {
        if (const T* type = std::get_if<T>(&_uniformData)) {
            OnImGuiForType(*type);
        }
    }

    template <typename Type>
    void ShaderUniform::OnImGuiForType(Type uniformData) {
        ImGui::Text(std::string(_uniformName + ':').c_str());

        // Type of uniform is bool.
        if constexpr (std::is_same_v<Type, bool>) {
            if (ImGui::Checkbox(_uniformImGuiLabel.c_str(), &uniformData)) {
                _uniformData = uniformData;
            }
        }
        // Type of uniform is int.
        else if constexpr (std::is_same_v<Type, int>) {
            if (ImGui::DragInt(_uniformImGuiLabel.c_str(), &uniformData)) {
                _uniformData = uniformData;
            }
        }
        // Type of uniform is float.
        else if constexpr (std::is_same_v<Type, float>) {
            if (ImGui::DragFloat(_uniformImGuiLabel.c_str(), &uniformData)) {
                _uniformData = uniformData;
            }
        }
        // Type of uniform is vec2.
        else if constexpr (std::is_same_v<Type, glm::vec2>) {
            if (ImGui::DragFloat2(_uniformImGuiLabel.c_str(), &uniformData[0], 0.1f, _minSliderRange, _maxSliderRange)) {
                _uniformData = uniformData;
            }
        }
        // Type of uniform is vec3.
        else if constexpr (std::is_same_v<Type, glm::vec3>) {
            if (_useColorPicker) {
                if (ImGui::ColorEdit3(_uniformImGuiLabel.c_str(), &uniformData[0])) {
                    _uniformData = uniformData;
                }
            }
            else {
                if (ImGui::DragFloat3(_uniformImGuiLabel.c_str(), &uniformData[0], 0.1f, _minSliderRange, _maxSliderRange)) {
                    _uniformData = uniformData;
                }
            }
        }
        // Type of uniform is vec4.
        else if constexpr (std::is_same_v<Type, glm::vec4>) {
            if (_useColorPicker) {
                if (ImGui::ColorEdit4(_uniformImGuiLabel.c_str(), &uniformData[0])) {
                    _uniformData = uniformData;
                }
            }
            else {
                if (ImGui::DragFloat4(_uniformImGuiLabel.c_str(), &uniformData[0], 0.1f, _minSliderRange, _maxSliderRange)) {
                    _uniformData = uniformData;
                }
            }
        }
        // Unsupported: mat3, mat4, TextureSampler
    }

}

#endif //SANDBOX_SHADER_UNIFORM_TPP