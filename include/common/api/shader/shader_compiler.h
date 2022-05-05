
#pragma once

#include "pch.h"
#include "common/utility/singleton.h"
#include "common/api/shader/shader_preprocessor.h"
#include "common/api/shader/shader_uniform_lut.h"

namespace Sandbox {

    using word = std::uint32_t;

    class ShaderCompiler : public ISingleton<ShaderCompiler> {
        public:
            REGISTER_SINGLETON(ShaderCompiler);

            struct ShaderInfo {
                ShaderInfo();

                std::string filepath;
                std::string workingDirectory;

                std::vector<word> binary;
            };

            [[nodiscard]] ShaderInfo ProcessFile(const ShaderPreprocessor::ShaderInfo& file);

        private:
            struct ProcessingContext {
                std::vector<std::string> errors;
                std::vector<std::string> warnings;
            };

            [[nodiscard]] bool CompileToSPIRV(const ShaderPreprocessor::ShaderInfo& file, ShaderInfo& info, ProcessingContext& context) const;
            [[nodiscard]] shaderc_profile ToSPIRVShaderProfile(ShaderProfile profile) const;
            [[nodiscard]] shaderc_shader_kind ToSPIRVShaderType(ShaderType type) const;

            [[nodiscard]] bool GenerateReflectionData(const ShaderPreprocessor::ShaderInfo& file, ShaderInfo& info, ProcessingContext& context) const;
    };

}