
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

            // Preprocesses inline shader source. Follows all rules that supported shader types (.vert, .frag, .geom, .tess, .comp, .glsl) have.
            std::unordered_map<ShaderType, ShaderInfo> ProcessSource(const std::string& source);

            // Adds a directory to look for shader includes in.
            void AddIncludeDirectory(const std::string& includeDirectory);

        private:
            struct ProcessingContext {
                std::vector<ShaderInclude> includeStack;
            };

            ShaderPreprocessor();
            ~ShaderPreprocessor() override;

            // Parses source code in 'source' and stores information into 'info'. Function should be called on individual shader types (.vert, .frag, .geom, .tess, .comp).
            bool ParseFile(const std::string& source, ShaderInfo& info, ProcessingContext& context);

            // Parses source code in 'source' and returns a mapping of all detected shader types.
            [[nodiscard]] std::unordered_map<ShaderType, ShaderInfo> ParseFile(const std::string& data, bool inlined);

            [[nodiscard]] std::string GetFormattedMessage(const ProcessingContext& context, const std::string& file, const std::string& line, unsigned lineNumber, const std::string& message, unsigned offset, unsigned length = 0) const;

            [[nodiscard]] bool ValidateShaderExtension(const std::string& in) const;
            [[nodiscard]] bool ValidateShaderVersion(int version) const;
            [[nodiscard]] bool ValidateShaderProfile(const std::string& in) const;
			[[nodiscard]] bool ValidateShaderType(const std::string& in) const;

            std::set<std::string> includeDirectories_;
    };

}

