
#pragma once

#include "pch.h"
#include "common/api/shader/shader_types.h"

namespace Sandbox {

    // Allows shaders to have includes.
    class ShaderParser {
        public:
            struct Result {
                explicit Result(const std::string& filepath);
                ~Result();

                std::string file;
                std::string directory;

                ShaderType type;
                std::string source;
                int version;

                bool success;
                std::vector<std::string> warnings;
                std::vector<std::string> errors;
            };

            ShaderParser();
            ~ShaderParser();

            [[nodiscard]] Result Parse(const std::string& filepath);

        private:
            struct ShaderInclude {
                ShaderInclude(const std::string& filename, unsigned lineNumber);
                ~ShaderInclude();

                [[nodiscard]] bool operator==(const ShaderInclude& other) const;

                std::string filename;
                unsigned lineNumber;
            };

            // Custom hashing functor for ShaderInclude.
            struct ShaderIncludeHash {
                std::size_t operator()(const ShaderInclude& data) const noexcept;
            };

            [[nodiscard]] std::string ProcessFile(const std::string& filepath, Result& out);
            [[nodiscard]] std::string GetLine(std::ifstream& stream) const;
            void ThrowFormattedError(const std::string& line, unsigned lineNumber, const std::string& message, int index) const;

            std::string path_;
            std::string baseDirectory_; // Directory parent shader file is based in.
            std::unordered_set<ShaderInclude, ShaderIncludeHash> dependencies_;
    };

}