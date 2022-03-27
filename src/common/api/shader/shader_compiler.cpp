
#include "common/api/shader/shader_compiler.h"
#include "common/utility/log.h"

namespace Sandbox {

    std::vector<std::uint32_t> ShaderCompiler::CompileToSPIRV(const ShaderInfo& info) {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        // TODO: Detect target environment from shader version (unsupported).

        // No difference between OpenGL 4.5 and 4.6 (from documentation).
        options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
        options.SetSourceLanguage(shaderc_source_language_glsl);
        options.SetForcedVersionProfile(info.version.version, ToSPIRVShaderProfile(info.profile));
        options.SetOptimizationLevel(shaderc_optimization_level_performance);

        shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(info.source, ToSPIRVShaderType(info.type), info.filepath.c_str(), "main", options);
        if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
            ImGuiLog::Instance().LogError("Compilation of shader file '%s' exited with %i error(s). Provided error message: %s", info.filepath.c_str(), module.GetNumErrors(), module.GetErrorMessage().c_str());
            throw std::runtime_error("ShaderCompiler::CompileToSPIRV call failed - see out/log.txt for more details.");
        }

        if (module.GetNumWarnings() > 0) {
            ImGuiLog::Instance().LogWarning("Compilation of shader file '%s' exited with %i warning(s).", info.filepath.c_str(), module.GetNumWarnings());
        }

        return { module.cbegin(), module.cend() };
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

    void ShaderCompiler::Reflect(const std::vector<std::uint32_t>& binary) {
        spirv_cross::CompilerGLSL compiler(binary);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        spirv_cross::CompilerGLSL::Options options;
        options.version
    }
}