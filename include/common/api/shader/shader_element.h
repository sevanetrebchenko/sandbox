
#pragma once

#include "pch.h"

namespace Sandbox {

    enum class ShaderContext {
        CORE,
        COMPATIBILITY,

        INVALID
    };

    [[nodiscard]] std::string ToString(ShaderContext profile);
    [[nodiscard]] ShaderContext ToShaderContext(const std::string& in);

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

    struct ShaderInclude {
        ShaderInclude(const std::string& filename, unsigned lineNumber);
        ~ShaderInclude();

        [[nodiscard]] bool operator==(const ShaderInclude& other) const;

        std::string filename;
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

#include "common/api/shader/shader_element.tpp"