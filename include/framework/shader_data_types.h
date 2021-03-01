
#ifndef SANDBOX_SHADER_DATA_TYPES_H
#define SANDBOX_SHADER_DATA_TYPES_H

namespace Sandbox {

    enum class ShaderDataType {
        NONE,
        BOOL, INT, FLOAT,
        VEC2, VEC3, VEC4,
        MAT4,
    };

    std::size_t ShaderDataTypeSize(ShaderDataType shaderDataType);

}

#endif //SANDBOX_SHADER_DATA_TYPES_H
