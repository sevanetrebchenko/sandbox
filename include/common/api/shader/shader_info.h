
#pragma once

#include "pch.h"
#include "shader_descriptor.h"

namespace Sandbox {

    // Information about a shader source file of one type.
    // Should only be used with concrete shader source types (.vert, .frag, .geom, .tess, .comp).
    struct ShaderInfo {
        ShaderInfo();

        std::string filepath;
        std::string workingDirectory;

        ShaderType type;
        ShaderProfile profile;
        ShaderVersion version;
        std::string source;
        std::unordered_set<ShaderInclude> dependencies;

        std::vector<std::uint32_t> binary;

        bool success; // Indicates whether shader file was preprocessed without errors.
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
    };

}