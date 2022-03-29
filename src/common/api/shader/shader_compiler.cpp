
#include "common/api/shader/shader_compiler.h"
#include "common/utility/log.h"

namespace Sandbox {

    ShaderCompiler::ShaderInfo::ShaderInfo() {
    }

    ShaderCompiler::ShaderInfo ShaderCompiler::ProcessFile(const ShaderPreprocessor::ShaderInfo& file) {
        ShaderInfo info { };
        ProcessingContext context { };

//        bool status = CompileToSPIRV(file, info, context);
//        if (!status) {
//            throw std::runtime_error("ShaderCompiler::ProcessFile call failed - see out/log.txt for more details.");
//        }

        return info;
    }

//    bool ShaderCompiler::CompileToSPIRV(const ShaderPreprocessor::ShaderInfo& file, ShaderInfo& info, ProcessingContext& context) const {
//        std::stringstream builder;
//
//        shaderc::CompileOptions options;
//        // No difference between OpenGL 4.5 and 4.6 (from documentation).
//        options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5); // TODO: Detect target environment from shader version (currently unsupported?).
//        options.SetSourceLanguage(shaderc_source_language_glsl);
//        options.SetForcedVersionProfile(file.version.version, ToSPIRVShaderProfile(file.profile));
//        options.SetOptimizationLevel(shaderc_optimization_level_performance);
//
//        shaderc::Compiler compiler;
//        shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(file.source, ToSPIRVShaderType(file.type), file.filepath.c_str(), "main", options);
//
//        if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
//            builder << "Compilation of shader file '" << file.filepath << "' exited with " << module.GetNumErrors() << " error(s). Provided error message: " << module.GetErrorMessage() << std::endl;
//            context.errors.emplace_back(builder.str());
//            return false;
//        }
//
//        if (module.GetNumWarnings() > 0) {
//            builder << "Compilation of shader file '" << file.filepath << "' exited with " << module.GetNumWarnings() << " warning(s)." << std::endl;
//            context.warnings.emplace_back(builder.str());
//        }
//
//        info.binary = { module.cbegin(), module.cend() };
//
//        return true;
//    }
//
//    shaderc_profile ShaderCompiler::ToSPIRVShaderProfile(ShaderProfile profile) const {
//        switch (profile) {
//            case ShaderProfile::CORE:
//                return shaderc_profile_core;
//            case ShaderProfile::COMPATIBILITY:
//                return shaderc_profile_compatibility;
//            default:
//            case ShaderProfile::INVALID:
//                throw std::runtime_error("Invalid ShaderProfile provided to ShaderCompiler::ToSPIRVShaderProfile.");
//        }
//    }
//
//    shaderc_shader_kind ShaderCompiler::ToSPIRVShaderType(ShaderType type) const {
//        switch (type) {
//            case ShaderType::VERTEX:
//                return shaderc_shader_kind::shaderc_glsl_vertex_shader;
//            case ShaderType::FRAGMENT:
//                return shaderc_shader_kind::shaderc_glsl_fragment_shader;
//            case ShaderType::GEOMETRY:
//                return shaderc_shader_kind::shaderc_glsl_geometry_shader;
//            case ShaderType::TESSELATION:
//                return shaderc_shader_kind::shaderc_glsl_tess_control_shader;
//            case ShaderType::COMPUTE:
//                return shaderc_shader_kind::shaderc_glsl_compute_shader;
//            default:
//            case ShaderType::INVALID:
//                throw std::runtime_error("Invalid ShaderType provided to ShaderCompiler::ToSPIRVShaderType.");
//        }
//    }


}