
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
        ShaderProfile profile; // Default (if not specified): CORE
        int version;
        std::string source;
        std::unordered_set<ShaderInclude> dependencies;
        unsigned numLines;

        bool success; // Indicates whether shader file was preprocessed without errors.
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
    };

    struct PreprocessResult {
        std::vector<ShaderInfo> data;
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

            [[nodiscard]] std::string GetFormattedErrorMessage(const std::string& file, unsigned lineNumber, const std::string& message) const;

            std::unordered_map<std::string, ShaderInfo> data_; // Cached information about parsed shader files.
            std::unordered_set<std::string> parsed_;

    };

}

