
#include "common/api/shader/shader_compiler.h"
#include "common/utility/log.h"

namespace Sandbox {

    ShaderCompiler::ShaderInfo::ShaderInfo() : filepath(""),
                                               workingDirectory("")
                                               {
    }

    ShaderCompiler::ShaderInfo ShaderCompiler::ProcessFile(const ShaderPreprocessor::ShaderInfo& file) {
        ShaderInfo info { };
        ProcessingContext context { };

        bool status = CompileToSPIRV(file, info, context);
        if (!status) {
            throw std::runtime_error("ShaderCompiler::ProcessFile call failed - see out/log.txt for more details.");
        }

        GenerateReflectionData(file, info, context);

        return info;
    }

    bool ShaderCompiler::CompileToSPIRV(const ShaderPreprocessor::ShaderInfo& file, ShaderInfo& info, ProcessingContext& context) const {
        std::stringstream builder;

        shaderc::CompileOptions options;
        // No difference between OpenGL 4.5 and 4.6 (from documentation).
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3); // TODO: Detect target environment from shader version (currently unsupported?).
        options.SetSourceLanguage(shaderc_source_language_glsl);
        options.SetForcedVersionProfile(file.version.version, ToSPIRVShaderProfile(file.profile));
        options.SetOptimizationLevel(shaderc_optimization_level_zero);

        // TODO: change from hard-coding "main" as entry point.
        shaderc::Compiler compiler;
        shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(file.source, ToSPIRVShaderType(file.type), file.filepath.c_str(), "main", options);

        if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
            builder << "Compilation of shader file '" << file.filepath << "' exited with " << module.GetNumErrors() << " error(s). Provided error message: " << module.GetErrorMessage() << std::endl;
            context.errors.emplace_back(builder.str());
            return false;
        }

        if (module.GetNumWarnings() > 0) {
            builder << "Compilation of shader file '" << file.filepath << "' exited with " << module.GetNumWarnings() << " warning(s)." << std::endl;
            context.warnings.emplace_back(builder.str());
        }

        info.filepath = file.filepath;
        info.workingDirectory = file.workingDirectory;
        info.binary = { module.cbegin(), module.cend() };

        return true;
    }

    shaderc_profile ShaderCompiler::ToSPIRVShaderProfile(ShaderProfile profile) const {
        switch (profile) {
            case ShaderProfile::CORE:
                return shaderc_profile_core;
            case ShaderProfile::COMPATIBILITY:
                return shaderc_profile_compatibility;
            default:
            case ShaderProfile::INVALID:
                throw std::runtime_error("Invalid ShaderProfile provided to ShaderCompiler::ToSPIRVShaderProfile.");
        }
    }

    shaderc_shader_kind ShaderCompiler::ToSPIRVShaderType(ShaderType type) const {
        switch (type) {
            case ShaderType::VERTEX:
                return shaderc_shader_kind::shaderc_glsl_vertex_shader;
            case ShaderType::FRAGMENT:
                return shaderc_shader_kind::shaderc_glsl_fragment_shader;
            case ShaderType::GEOMETRY:
                return shaderc_shader_kind::shaderc_glsl_geometry_shader;
            case ShaderType::TESSELATION:
                return shaderc_shader_kind::shaderc_glsl_tess_control_shader;
            case ShaderType::COMPUTE:
                return shaderc_shader_kind::shaderc_glsl_compute_shader;
            default:
            case ShaderType::INVALID:
                throw std::runtime_error("Invalid ShaderType provided to ShaderCompiler::ToSPIRVShaderType.");
        }
    }

    bool ShaderCompiler::GenerateReflectionData(const ShaderPreprocessor::ShaderInfo& file, ShaderCompiler::ShaderInfo& info, ShaderCompiler::ProcessingContext& context) const {
        spirv_cross::CompilerGLSL compiler(info.binary);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        // Shader inputs.
        std::cout << "shader inputs: " << std::endl;
        for (const spirv_cross::Resource& input: resources.stage_inputs) {
            std::cout << compiler.get_name(input.type_id) << std::endl;
        }
        std::cout << std::endl;

        // Shader outputs.
        std::cout << "shader outputs: " << std::endl;
        for (const spirv_cross::Resource& output: resources.stage_outputs) {
            std::cout << compiler.get_name(output.id) << std::endl;
        }
        std::cout << std::endl;

        for (const spirv_cross::Resource& resource: resources.uniform_buffers) {
            const spirv_cross::SPIRType& type = compiler.get_type(resource.base_type_id);
            unsigned size = compiler.get_declared_struct_size(type);
            unsigned set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
            unsigned binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            unsigned location = compiler.get_decoration(resource.id, spv::DecorationLocation);
            unsigned memberCount = type.member_types.size();

            std::cout << compiler.get_name(resource.id) << std::endl;

            for (int member = 0; member < memberCount; ++member) {
                std::cout << compiler.get_member_name(resource.base_type_id, member) << std::endl;
            }

            std::cout << "Found shader resource: " << resource.name << std::endl;
            std::cout << "\tsize: " << size << std::endl;
            std::cout << "\tbinding: " << binding << std::endl;
            std::cout << "\tmembers: " << memberCount << std::endl;
        }

        spirv_cross::CompilerGLSL::Options options;
        options.version = file.version.version;
        options.es = false;

        compiler.set_common_options(options);

        info.glsl = compiler.compile();

        return true;
    }

}