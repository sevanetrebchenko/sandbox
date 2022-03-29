
#pragma once

#include "pch.h"
#include "common/api/shader/shader_descriptor.h"
#include "common/utility/singleton.h"

namespace Sandbox {

    // Preprocessing inlined shader source files is not supported, as shaders should be reloadable entities.
    class ShaderPreprocessor : public ISingleton<ShaderPreprocessor> {
        public:
            REGISTER_SINGLETON(ShaderPreprocessor);

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
            };

            // Preprocesses individual shader files (.vert, .frag, .geom, .comp, .tess, etc.).
            [[nodiscard]] ShaderInfo ProcessFile(const std::string& filepath);

            // Adds a directory to look for shader includes in.
            void AddIncludeDirectory(const std::string& includeDirectory);

        private:
            struct ProcessingContext {
                std::vector<ShaderInclude> includeStack;
                std::vector<std::string> errors;
                std::vector<std::string> warnings;
            };

            ShaderPreprocessor();
            ~ShaderPreprocessor() override;

            // Parses source code in 'source' and stores information into 'info'.
            bool ParseFile(const std::string& source, ShaderInfo& info, ProcessingContext& context);

            [[nodiscard]] std::string GetFormattedMessage(const ProcessingContext& context, const std::string& file, const std::string& line, unsigned lineNumber, const std::string& message, unsigned offset, unsigned length = 0) const;

            [[nodiscard]] bool ValidateShaderExtension(const std::string& in) const;
            [[nodiscard]] bool ValidateShaderVersion(int version) const;
            [[nodiscard]] bool ValidateShaderProfile(const std::string& in) const;
			[[nodiscard]] bool ValidateShaderType(const std::string& in) const;

            std::set<std::string> includeDirectories_;
    };

}

