
#ifndef SANDBOX_SHADER_UNIFORM_H
#define SANDBOX_SHADER_UNIFORM_H

#include <sandbox_pch.h>
#include <framework/texture.h>
#include <framework/shader.h>

#define SUPPORTED_UNIFORM_TYPES bool, int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4, TextureSampler

namespace Sandbox {

    class ShaderUniform {
        public:
            typedef std::variant<SUPPORTED_UNIFORM_TYPES> UniformEntry;

            ShaderUniform(std::string uniformName, UniformEntry uniformData);
            ShaderUniform(const ShaderUniform& other);

            void OnImGui();

            void Bind(Shader* shaderProgram) const;
            void Unbind() const;

            void SetSliderRange(float min, float max);
            void UseColorPicker(bool colorPicker);

            [[nodiscard]] const std::string& GetName() const;

            [[nodiscard]] UniformEntry& GetData();

            template <typename T>
            void SetData(const T& data);

        private:
            template <typename Type, typename T1, typename ...T2>
            void SetDataHelper(const Type& data);

            template <typename Type, typename T1>
            void SetDataHelper(const Type& data);

            template <typename T>
            void TrySetData(const T& data);

            template <typename T1, typename T2, typename ...T3>
            void BindHelper(Shader* shaderProgram) const;

            template <typename T>
            void BindHelper(Shader* shaderProgram) const;

            template <typename T1, typename T2, typename ...T3>
            void OnImGuiHelper();

            template <typename T>
            void OnImGuiHelper();

            template <typename Type>
            void OnImGuiForType(Type uniformData);

            std::string _uniformName;
            std::string _uniformImGuiLabel;
            UniformEntry _uniformData;

            // ImGui configuration.
            bool _useColorPicker;
            float _minSliderRange;
            float _maxSliderRange;
    };

}

#include <framework/shader_uniform.tpp>

#endif //SANDBOX_SHADER_UNIFORM_H

