
#pragma once

#include "pch.h"
#include "common/api/shader/shader_descriptor.h"
#include "common/utility/singleton.h"

namespace Sandbox {



    // Preprocessing inlined shader source files is not supported, as shaders should be reloadable entities.
    class ShaderPreprocessor : public ISingleton<ShaderPreprocessor> {
        public:
            REGISTER_SINGLETON(ShaderPreprocessor);

            // Preprocesses individual shader files (.vert, .frag, .geom, .comp, .tess, etc.) or joint shader files (.glsl).
            // Returns mapping of shader type to shader source for each parsed type.
            std::unordered_map<ShaderType, ShaderInfo> ProcessFile(const std::string& filepath);

            // Adds a directory to look for shader includes in.
            void AddIncludeDirectory(const std::string& includeDirectory);

        private:
            struct ProcessingContext {
                std::vector<ShaderInclude> includeStack;
            };

            ShaderPreprocessor();
            ~ShaderPreprocessor() override;

            // Parses source code in file 'filepath' and returns a mapping of all detected shader types.
            [[nodiscard]] std::unordered_map<ShaderType, ShaderInfo> ParseFile(const std::string& filepath);

            // Parses source code in 'source' and stores information into 'info'. Function should be called on individual
            // shader types (.vert, .frag, .geom, .tess, .comp).
            bool ParseFile(const std::string& source, ShaderInfo& info, ProcessingContext& context);

            [[nodiscard]] std::string GetFormattedMessage(const ProcessingContext& context, const std::string& file, const std::string& line, unsigned lineNumber, const std::string& message, unsigned offset, unsigned length = 0) const;

            [[nodiscard]] bool ValidateShaderExtension(const std::string& in) const;
            [[nodiscard]] bool ValidateShaderVersion(int version) const;
            [[nodiscard]] bool ValidateShaderProfile(const std::string& in) const;
			[[nodiscard]] bool ValidateShaderType(const std::string& in) const;

            std::set<std::string> includeDirectories_;
    };

}

