
#pragma once

#include <vector>
#include <string>
#include <shaderc/shaderc.hpp>
#include <spirv_glsl.hpp>

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

            template <typename T>
            [[nodiscard]] IDataType* ConstructPrimitiveType(unsigned numElements) const;

            template <typename T>
            [[nodiscard]] IDataType* ConstructPrimitiveType(unsigned numElements, unsigned numColumns) const;

            [[nodiscard]] std::vector<IDataType*> EnumerateStructMembers(const spirv_cross::CompilerGLSL& compiler, const spirv_cross::SPIRType& structType) const;
            [[nodiscard]] IDataType* ReflectDataType(const spirv_cross::CompilerGLSL& compiler, const spirv_cross::SPIRType& declaredType, const spirv_cross::SPIRType& modifiedType) const;
    };

}

#include "common/api/shader/shader_compiler.tpp"