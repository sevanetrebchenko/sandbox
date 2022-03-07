
#ifndef SANDBOX_SHADER_TYPES_H
#define SANDBOX_SHADER_TYPES_H

#include "pch.h"

namespace Sandbox {

    enum class ShaderDataType {
        NONE,
        BOOL, INT, UINT, FLOAT,
        VEC2, IVEC2, VEC3, UVEC3, VEC4, UVEC4,
        MAT4,
    };

    std::size_t ShaderDataTypeSize(ShaderDataType shaderDataType);

}

#endif //SANDBOX_SHADER_TYPES_H
