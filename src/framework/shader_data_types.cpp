
#include <framework/shader_data_types.h>

namespace Sandbox {

    std::size_t ShaderDataTypeSize(ShaderDataType shaderDataType) {
        switch (shaderDataType) {
            case ShaderDataType::INT:
                return sizeof(int);
            case ShaderDataType::BOOL:
                return sizeof(bool);
            case ShaderDataType::FLOAT:
                return sizeof(float);
            case ShaderDataType::VEC2:
                return sizeof(glm::vec2);
            case ShaderDataType::VEC3:
                return sizeof(glm::vec3);
            case ShaderDataType::VEC4:
                return sizeof(glm::vec4);
            case ShaderDataType::MAT4:
                return sizeof(glm::mat4);
            default:
                throw std::runtime_error("Unknown shader data type provided to ShaderDataTypeSize.");
        }
    }

}

