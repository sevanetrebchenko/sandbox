
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

    struct FileInfo {
        FileInfo();
        FileInfo(std::string file, unsigned lineNumber);

        std::string file;
        unsigned lineNumber;
    };

    struct ShaderVersion {
        ShaderVersion();
        ShaderVersion(FileInfo location, int version);
        ShaderVersion(std::string file, unsigned lineNumber, int version);

        [[nodiscard]] bool operator==(const ShaderVersion& other) const;
        [[nodiscard]] bool operator!=(const ShaderVersion& other) const;

        operator int() const;

        FileInfo location;
        int version;
    };


    struct ShaderInclude {
        ShaderInclude();
        ShaderInclude(FileInfo parent, std::string file);
        ShaderInclude(std::string parentFile, unsigned lineNumber, std::string file);

        [[nodiscard]] bool operator==(const ShaderInclude& other) const;
        [[nodiscard]] bool operator!=(const ShaderInclude& other) const;

        FileInfo parent;
        std::string file;
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