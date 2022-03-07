
#pragma once

#include "pch.h"
#include "common/api/shader/shader_element.h"
#include "common/utility/singleton.h"

namespace Sandbox {

    // Information about a shader source file of one type.
    struct ShaderInfo {
        explicit ShaderInfo(const std::string& filepath);

        std::string filepath;
        std::string workingDirectory; // Base directory of shader file - allows for local shader includes.

        ShaderType type;
        ShaderContext context; // Default (if not specified): CORE
        int version;
        std::string source;
        std::unordered_set<ShaderInclude> dependencies;
        unsigned numLines;

        bool success; // Indicates whether shader file was preprocessed without errors.
        std::vector<std::string> warnings;
        std::vector<std::string> errors;

        std::filesystem::file_time_type lastEditTime;
    };

    class ShaderPreprocessor : public Singleton<ShaderPreprocessor> {
        public:
            REGISTER_SINGLETON(ShaderPreprocessor);

            // Preprocesses individual shader files (.vert, .frag, .geom, .comp, .tess, etc.) or joint shader files (.glsl).
            void Process(const std::string& filepath);

        private:
            ShaderPreprocessor();
            ~ShaderPreprocessor() override;

            // Processes shader file and caches any new dependencies.
            void ReadFile(const std::string& filepath);

            [[nodiscard]] std::string GetLine(std::ifstream& in) const;

            // Removes only newlines from the input string.
            [[nodiscard]] std::string RemoveNewlines(const std::string& in) const;

            // Removes all whitespace characters (\n, ' ', \r, \t, \f, \v) from the input string.
            [[nodiscard]] std::string RemoveWhitespace(const std::string& in) const;

            [[nodiscard]] std::string GetFormattedMessage(const std::string& file, unsigned lineNumber, const std::string& message) const;

            [[nodiscard]] bool ValidateShaderExtension(const std::string& in) const;
            [[nodiscard]] bool ValidateShaderVersion(int version) const;
			[[nodiscard]] bool ValidateShaderType(const std::string& in) const;

            std::unordered_map<std::string, ShaderInfo> parsed_; // Cached information about parsed shader files.
    };

}

