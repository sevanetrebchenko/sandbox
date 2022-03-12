
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
        [[nodiscard]] bool operator==(const ShaderVersion& other) const;
        [[nodiscard]] bool operator!=(const ShaderVersion& other) const;

        operator int() const;

        int version;

        std::string file;
        unsigned lineNumber;
    };


    struct ShaderInclude {
        [[nodiscard]] bool operator==(const ShaderInclude& other) const;
        [[nodiscard]] bool operator!=(const ShaderInclude& other) const;

        std::string parentFile; // File containing the included file.
        unsigned lineNumber;

        std::string file; // Filepath of included file.
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