
#include <framework/shader_uniform.h>
#define SUPPORTED_UNIFORM_TYPES bool, int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4

namespace Sandbox {

    ShaderUniform::ShaderUniform(std::string uniformName, ShaderUniform::UniformEntry uniformData, bool useColorPicker) : _uniformName(std::move(uniformName)),
                                                                                                                          _uniformData(uniformData),
                                                                                                                          _uniformImGuiLabel(std::string("##" + _uniformName)),
                                                                                                                          _useColorPicker(useColorPicker),
                                                                                                                          _minSliderRange(0.0f),
                                                                                                                          _maxSliderRange(10.0f)
                                                                                                                          {

    }

    ShaderUniform::ShaderUniform(const ShaderUniform &other) {
        _uniformName = other._uniformName;
        _uniformImGuiLabel = other._uniformImGuiLabel;
        _uniformData = other._uniformData;

        _useColorPicker = other._useColorPicker;
        _minSliderRange = other._minSliderRange;
        _maxSliderRange = other._maxSliderRange;
    }

    void ShaderUniform::OnImGui() {
        OnImGuiHelper<SUPPORTED_UNIFORM_TYPES>();
    }

    void ShaderUniform::Bind(Shader *shaderProgram) const {
        BindHelper<SUPPORTED_UNIFORM_TYPES>(shaderProgram);
    }

    void ShaderUniform::Unbind() const {
        // Nothing.
    }

    void ShaderUniform::UseColorPicker(bool colorPicker) {
        _useColorPicker = colorPicker;
    }

    void ShaderUniform::SetSliderRange(float min, float max) {
        _minSliderRange = min;
        _maxSliderRange = max;
    }


}
