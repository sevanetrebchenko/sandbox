
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


    // Shader types.
    class ShaderType {
        public:
            explicit ShaderType(GLenum type);
            explicit ShaderType(const std::string& filepath);

            ~ShaderType();

            [[nodiscard]] GLenum ToOpenGLType() const;
            [[nodiscard]] std::string ToString() const;
            [[nodiscard]] shaderc_shader_kind ToSPIRVType() const;

        private:
            GLenum openGL_;
            shaderc_shader_kind spirv_;

            std::string extension_;
    };

}

#endif //SANDBOX_SHADER_TYPES_H
