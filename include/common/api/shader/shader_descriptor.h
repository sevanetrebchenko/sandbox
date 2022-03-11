
#pragma once

#include "pch.h"

namespace Sandbox {

    // https://www.khronos.org/opengl/wiki/OpenGL_Context
    enum class ShaderProfile {
        CORE,
        COMPATIBILITY,

        INVALID
    };

    [[nodiscard]] std::string ToString(ShaderProfile profile);
    [[nodiscard]] ShaderProfile ToShaderProfile(const std::string& in);

    enum class ShaderType {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        TESSELATION,
        COMPUTE,

        INVALID
    };

    [[nodiscard]] std::string ToString(ShaderType type);
    [[nodiscard]] ShaderType ToShaderType(const std::string& in);

    struct ShaderVersion {
        ShaderVersion(int version, unsigned lineNumber);
        ~ShaderVersion();

        [[nodiscard]] bool operator==(const ShaderVersion& other) const;
        [[nodiscard]] bool operator!=(const ShaderVersion& other) const;

        int data;
        unsigned lineNumber;
    };


    struct ShaderInclude {
        ShaderInclude(const std::string& filepath, unsigned lineNumber);
        ~ShaderInclude();

        [[nodiscard]] bool operator==(const ShaderInclude& other) const;
        [[nodiscard]] bool operator!=(const ShaderInclude& other) const;

        std::string filepath;
        unsigned lineNumber;
    };

}

namespace std {

    // Custom hashing function for ShaderInclude.
    template <>
    struct hash<Sandbox::ShaderInclude> {
        std::size_t operator()(const Sandbox::ShaderInclude& data) const;
    };

}

#include "common/api/shader/shader_descriptor.tpp"