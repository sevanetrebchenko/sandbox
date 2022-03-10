
#pragma once

#include "pch.h"
#include "common/api/shader/shader_types.h"
#include "common/utility/directory.h"

namespace Sandbox {

    class ShaderComponent {
        public:
            struct CompilationResult {
                CompilationResult();

                bool success;
                GLint ID;
                std::vector<std::uint32_t> source;
                unsigned numWarnings;
                unsigned numErrors;
            };

            explicit ShaderComponent(const std::string& filepath);
            ~ShaderComponent() = default;

            // Returns the ID of the shader component.
            [[nodiscard]] CompilationResult Compile();

            [[nodiscard]] int GetVersion() const;
            [[nodiscard]] const std::string& GetSource() const;

        private:
            struct ShaderInclude {
                ShaderInclude(const std::string& filename, unsigned lineNumber);
                ~ShaderInclude();

                [[nodiscard]] bool operator==(const ShaderInclude& other) const;

                std::string filename_;
                unsigned lineNumber_;
            };

            // Custom hashing functor for ShaderInclude.
            struct ShaderIncludeHash {
                std::size_t operator()(const ShaderInclude& data) const noexcept;
            };

            struct ProcessResult {
                ProcessResult();

                unsigned numWarnings;
                unsigned numErrors;
                std::string source;
            };

            [[nodiscard]] std::string Process(const std::string& filepath);
            [[nodiscard]] std::string GetLine(std::ifstream& stream) const;
            void ThrowFormattedError(const std::string& line, unsigned lineNumber, const std::string& message, int index) const;

            std::string path_;
            std::string baseDirectory_; // Directory parent shader file is based in.
            int version_;
            std::string source_;
            std::unordered_set<ShaderInclude, ShaderIncludeHash> dependencies_;
    };

}





