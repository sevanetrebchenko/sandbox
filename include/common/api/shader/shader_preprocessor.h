
#pragma once

#include "pch.h"
#include "common/api/shader/shader_descriptor.h"
#include "common/utility/singleton.h"

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

        bool success; // Indicates whether shader file was preprocessed without errors.
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
    };

    class ShaderPreprocessor : public Singleton<ShaderPreprocessor> {
        public:
            REGISTER_SINGLETON(ShaderPreprocessor);

            // Preprocesses individual shader files (.vert, .frag, .geom, .comp, .tess, etc.) or joint shader files (.glsl).
            // Returns mapping of shader type to shader source for each parsed type.
            std::unordered_map<ShaderType, ShaderInfo> ProcessFile(const std::string& filepath);

            // Adds a directory to look for shader includes in.
            void AddIncludeDirectory(const std::string& includeDirectory);

        private:
            // Custom string tokenizer that does not ignore whitespace characters.
            class Tokenizer {
                public:
                    struct Token {
                        Token();
                        explicit Token(std::string in);
                        ~Token();

                        [[nodiscard]] unsigned Size() const;

                        std::string data; // Token with no whitespace.
                        unsigned length;  // Size of data (cached).
                        unsigned before;  // Number of whitespace characters before token (if any).
                        unsigned after;   // Number of whitespace characters after token (if any).
                    };

                    Tokenizer();
                    ~Tokenizer();

                    void Set(std::string in);
                    [[nodiscard]] bool IsValid() const;
                    [[nodiscard]] Token Next();

                private:
                    std::string line;
                    unsigned size;
                    unsigned index;
            };

            struct ProcessingContext {
                std::vector<ShaderInclude> includeStack;
            };

            ShaderPreprocessor();
            ~ShaderPreprocessor() override;

            // Reads file specified by data set in 'info'
            bool ReadFile(ShaderInfo& info, ProcessingContext& context);
            [[nodiscard]] std::unordered_map<ShaderType, ShaderInfo> ReadFile(const std::string& filepath);

            [[nodiscard]] std::string GetFormattedMessage(const ProcessingContext& context, const std::string& file, const std::string& line, unsigned lineNumber, const std::string& message, unsigned offset, unsigned length = 0) const;

            [[nodiscard]] bool ValidateShaderExtension(const std::string& in) const;
            [[nodiscard]] bool ValidateShaderVersion(int version) const;
            [[nodiscard]] bool ValidateShaderProfile(const std::string& in) const;
			[[nodiscard]] bool ValidateShaderType(const std::string& in) const;

            std::set<std::string> includeDirectories_;
    };

}

