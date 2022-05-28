
#pragma once

#include "pch.h"
#include "common/utility/singleton.h"
#include "common/api/shader/shader_preprocessor.h"
#include "common/api/shader/shader_uniform_lut.h"

namespace Sandbox {

    class ShaderCompiler : public ISingleton<ShaderCompiler> {
        public:
            REGISTER_SINGLETON(ShaderCompiler);

            struct ShaderInfo {
                ShaderInfo();

                std::string filepath;
                std::string workingDirectory;

                std::vector<unsigned> binary; // SPIR-V Binary.
                std::string glsl;
            };

            [[nodiscard]] ShaderInfo ProcessFile(const ShaderPreprocessor::ShaderInfo& file);

        private:
            struct ProcessingContext {
                std::vector<std::string> errors;
                std::vector<std::string> warnings;

                std::vector<unsigned> spirv;
            };

            void CompileToSPIRV(const ShaderPreprocessor::ShaderInfo& file, ProcessingContext& context, const shaderc::CompileOptions& options) const;
            [[nodiscard]] shaderc_profile ToSPIRVShaderProfile(ShaderProfile profile) const;
            [[nodiscard]] shaderc_shader_kind ToSPIRVShaderType(ShaderType type) const;

            void GenerateReflectionData(const ShaderPreprocessor::ShaderInfo& file, ShaderInfo& info, const std::vector<unsigned>& binary) const;
            void ReflectVertexShader() const;
    };

}