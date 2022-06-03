
#include "common/api/shader/shader_compiler.h"
#include "common/api/shader/shader_types.h"
#include "common/api/shader/descriptor/shader_input.h"
#include "common/utility/log.h"
#include "common/api/shader/shader_types.h"

namespace Sandbox {

    ShaderCompiler::ShaderInfo::ShaderInfo() : filepath(""),
                                               workingDirectory("")
                                               {
    }

    ShaderCompiler::ShaderInfo ShaderCompiler::ProcessFile(const ShaderPreprocessor::ShaderInfo& file) {
        std::stringstream builder;

        ShaderInfo info { };
        info.filepath = file.filepath;
        info.workingDirectory = file.workingDirectory;

        ProcessingContext context { };

        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3); // Vulkan GLSL.
//        options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5); // Vulkan GLSL.
        options.SetSourceLanguage(shaderc_source_language_glsl);
        options.SetForcedVersionProfile(file.version.version, ToSPIRVShaderProfile(file.profile));

        // Compile optimized SPIR-V shader code.
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
        CompileToSPIRV(file, context, options);

        if (!context.errors.empty()) {
            builder << "Compilation of shader file '" << file.filepath << "' exited with " << context.errors.size() << " error(s):" << std::endl;
            for (const std::string& message : context.errors) {
                builder << message;
            }
            builder << std::endl;

            ImGuiLog::Instance().LogError("%s", builder.str().c_str());
            throw std::runtime_error("ShaderCompiler::ProcessFile call failed - see out/log.txt for more details.");
        }

        if (!context.warnings.empty()) {
            builder << "Compilation of shader file '" << file.filepath << "' exited with " << context.warnings.size() << " warning(s):" << std::endl;
            for (const std::string& message : context.warnings) {
                builder << message;
            }
            builder << std::endl;

            ImGuiLog::Instance().LogWarning("%s", builder.str().c_str());
        }

        info.binary = context.spirv;

        // Compile non-optimized shader code for reflection.
        options.SetOptimizationLevel(shaderc_optimization_level_zero);
        CompileToSPIRV(file, context, options);

        GenerateReflectionData(file, info, context.spirv);

        return info;
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
            case ShaderType::TESSELATION_CONTROL:
                return shaderc_shader_kind::shaderc_glsl_tess_control_shader;
            case ShaderType::TESSELATION_EVALUATION:
                return shaderc_shader_kind::shaderc_glsl_default_tess_evaluation_shader;
            case ShaderType::COMPUTE:
                return shaderc_shader_kind::shaderc_glsl_compute_shader;
            default:
                throw std::runtime_error("Invalid ShaderType provided to ShaderCompiler::ToSPIRVShaderType.");
        }
    }

    void ShaderCompiler::GenerateReflectionData(const ShaderPreprocessor::ShaderInfo& file, ShaderCompiler::ShaderInfo& info, const std::vector<unsigned>& binary) const {
        spirv_cross::CompilerGLSL compiler(binary);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        // Shader inputs.
        std::vector<ShaderInput> inputs;

        switch (file.type) {
            case ShaderType::VERTEX:
                for (const spirv_cross::Resource& resource : resources.stage_inputs) {
                    // https://www.khronos.org/opengl/wiki/Vertex_Shader#Inputs

                    // Format:
                    // layout (location = LOCATION) in TYPE NAME;
                    const std::string& name = resource.name;
                    const spirv_cross::SPIRType& declaredType = compiler.get_type(resource.base_type_id);
                    const spirv_cross::SPIRType& modifiedType = compiler.get_type(resource.type_id);

                    IDataType* type = ReflectDataType(compiler, declaredType, modifiedType);

                    // Reflect vertex attribute indices.
                    std::optional<unsigned> location;

                    if (compiler.has_decoration(resource.id, spv::DecorationLocation)) {
                        // Attribute indices do not have to be assigned in the shader itself, either via a call to glBindAttribLocation
                        // before shader program linking or automatically assigned by OpenGL when the program is linked.
                        location = compiler.get_decoration(resource.id, spv::DecorationLocation);
                        std::cout << name << " has location " << std::to_string(location.value()) << std::endl;
                    }
                }

                for (const spirv_cross::Resource& resource : resources.stage_outputs) {
                    // https://www.khronos.org/opengl/wiki/Vertex_Shader#Inputs

                    const std::string& name = resource.name;
                    const spirv_cross::SPIRType& declaredType = compiler.get_type(resource.base_type_id);
                    const spirv_cross::SPIRType& modifiedType = compiler.get_type(resource.type_id);
                }

                for (const spirv_cross::Resource& resource : resources.uniform_buffers) {
                    const spirv_cross::SPIRType& declaredType = compiler.get_type(resource.base_type_id);
                    const spirv_cross::SPIRType& modifiedType = compiler.get_type(resource.type_id);

                    IDataType* type = ReflectDataType(compiler, declaredType, modifiedType);

                    // THIS WORKS.
                    auto flags = compiler.get_buffer_block_flags(resource.id);
                    bool cm = flags.get(spv::DecorationColMajor);
                    bool rm = flags.get(spv::DecorationRowMajor);

                    // THIS WORKS.
                    unsigned mc = compiler.get_type(resource.type_id).member_types.size();
                    for (int i = 0; i < mc; ++i) {
                        const std::string& name = compiler.get_member_name(resource.base_type_id, i);
                        bool r = compiler.get_member_decoration(resource.base_type_id, i, spv::DecorationRowMajor);
                        bool c = compiler.get_member_decoration(resource.base_type_id, i, spv::DecorationColMajor);
                        std::cout << "name: " << name << ", column major? " << (c ? "yes" : "no") << ", row major? " << (r ? "yes" : "no") << std::endl;
                    }
                }

//                for (const spirv_cross::Resource& resource : resources.storage_buffers) {
//                    if (compiler.has_decoration(resource.id, spv::DecorationRowMajor)) {
//                        const spirv_cross::SPIRType& type = compiler.get_type(resource.base_type_id);
//                        unsigned columnMajor = compiler.get_decoration(resource.id, spv::DecorationColMajor);
//                    }
//
//                    unsigned c = compiler.get_decoration(resource.id, spv::DecorationColMajor);
//
//                    unsigned memberCount = compiler.get_type(resource.base_type_id).member_types.size();
//                    for (int member = 0; member < memberCount; ++member) {
//
//                        std::cout << compiler.get_member_name(resource.base_type_id, member) << std::endl;
//
//                        if (compiler.has_decoration(compiler.get_type(resource.base_type_id).member_types[member], spv::DecorationRowMajor)) {
//                            unsigned columnMajor = compiler.get_decoration(resource.id, spv::DecorationColMajor);
//                        }
//                    }
//                }
                break;
            case ShaderType::FRAGMENT:
                for (const spirv_cross::Resource& resource : resources.stage_inputs) {
                    // Format:
                    // layout (location = LOCATION) in TYPE NAME;
                    const std::string& name = resource.name;
                    const spirv_cross::SPIRType& type = compiler.get_type(resource.base_type_id);

                    // Before OpenGL 4.3, matching interfaces required interpolation qualifiers to be present on other stages,
                    // but interpolation is controlled entirely by the qualifies on the input variables to the fragment shader.
                    unsigned noPerspective = compiler.get_decoration(resource.id, spv::DecorationNoPerspective);
                    unsigned flat = compiler.get_decoration(resource.id, spv::DecorationFlat);
                    unsigned centroid = compiler.get_decoration(resource.id, spv::DecorationCentroid);
                    unsigned sample = compiler.get_decoration(resource.id, spv::DecorationSample);

                    // Reflect vertex attribute indices.
                    unsigned location = compiler.get_decoration(resource.id, spv::DecorationLocation);
                }
                break;
            case ShaderType::GEOMETRY:
                break;
            case ShaderType::TESSELATION_CONTROL:
                break;
            case ShaderType::TESSELATION_EVALUATION:
                break;
            case ShaderType::COMPUTE:
                break;
            case ShaderType::INVALID:
                break;
        }

        for (const spirv_cross::Resource& resource : resources.stage_inputs) {
            ShaderInput& input = inputs.emplace_back();

//            std::cout << input.name << std::endl;
//            const spirv_cross::SPIRType& type = compiler.get_type(input.base_type_id);

//            switch (type.basetype) {
//                case spirv_cross::SPIRType::Void:
//                    break;
//                case spirv_cross::SPIRType::Boolean:
//                    break;
//                case spirv_cross::SPIRType::SByte:
//                    break;
//                case spirv_cross::SPIRType::UByte:
//                    break;
//                case spirv_cross::SPIRType::Short:
//                    break;
//                case spirv_cross::SPIRType::UShort:
//                    break;
//                case spirv_cross::SPIRType::Int:
//                    break;
//                case spirv_cross::SPIRType::UInt:
//                    break;
//                case spirv_cross::SPIRType::Int64:
//                    break;
//                case spirv_cross::SPIRType::UInt64:
//                    break;
//                case spirv_cross::SPIRType::AtomicCounter:
//                    break;
//                case spirv_cross::SPIRType::Half:
//                case spirv_cross::SPIRType::Float:
//                    break;
//                case spirv_cross::SPIRType::Double:
//                    break;
//                case spirv_cross::SPIRType::Struct:
//                    break;
//                case spirv_cross::SPIRType::Image:
//                case spirv_cross::SPIRType::SampledImage:
//                    break;
//                case spirv_cross::SPIRType::Sampler:
//                case spirv_cross::SPIRType::AccelerationStructure:
//                case spirv_cross::SPIRType::RayQuery:
//                case spirv_cross::SPIRType::ControlPointArray:
//                case spirv_cross::SPIRType::Interpolant:
//                case spirv_cross::SPIRType::Char:
//                case spirv_cross::SPIRType::Unknown:
//                    break;
//            }

        }
        std::cout << std::endl;

        // Shader outputs.
        std::cout << "shader outputs: " << std::endl;
        for (const spirv_cross::Resource& output : resources.stage_outputs) {
            std::cout << output.name << std::endl;
            const spirv_cross::SPIRType& type = compiler.get_type(output.base_type_id);

            switch (type.basetype) {
                case spirv_cross::SPIRType::Void:
                    break;
                case spirv_cross::SPIRType::Boolean:
                    break;
                case spirv_cross::SPIRType::SByte:
                    break;
                case spirv_cross::SPIRType::UByte:
                    break;
                case spirv_cross::SPIRType::Short:
                    break;
                case spirv_cross::SPIRType::UShort:
                    break;
                case spirv_cross::SPIRType::Int:
                    break;
                case spirv_cross::SPIRType::UInt:
                    break;
                case spirv_cross::SPIRType::Int64:
                    break;
                case spirv_cross::SPIRType::UInt64:
                    break;
                case spirv_cross::SPIRType::AtomicCounter:
                    break;
                case spirv_cross::SPIRType::Half:
                case spirv_cross::SPIRType::Float:
                    break;
                case spirv_cross::SPIRType::Double:
                    break;
                case spirv_cross::SPIRType::Struct:
                    break;
                case spirv_cross::SPIRType::Image:
                case spirv_cross::SPIRType::SampledImage:
                    break;
                case spirv_cross::SPIRType::Sampler:
                case spirv_cross::SPIRType::AccelerationStructure:
                case spirv_cross::SPIRType::RayQuery:
                case spirv_cross::SPIRType::ControlPointArray:
                case spirv_cross::SPIRType::Interpolant:
                case spirv_cross::SPIRType::Char:
                case spirv_cross::SPIRType::Unknown:
                    break;
            }

        }
        std::cout << std::endl;

        for (const spirv_cross::EntryPoint& entryPoint : compiler.get_entry_points_and_stages()) {
            std::cout << entryPoint.name << std::endl;
        }

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
        options.es = false; // TODO:
        // Compile back to GLSL.
        compiler.set_common_options(options);


        std::string glsl = compiler.compile();
        std::cout << glsl << std::endl;
        return;
    }

    void ShaderCompiler::CompileToSPIRV(const ShaderPreprocessor::ShaderInfo& file, ShaderCompiler::ProcessingContext& context, const shaderc::CompileOptions& options) const {
        std::stringstream builder;

        // TODO: change from hard-coding "main" as entry point.
        shaderc::Compiler compiler;
        shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(file.source, ToSPIRVShaderType(file.type), file.filepath.c_str(), "main", options);

        if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
            builder << "Compilation of shader file '" << file.filepath << "' exited with " << module.GetNumErrors() << " error(s). Provided error message: " << module.GetErrorMessage() << std::endl;
            context.errors.emplace_back(builder.str());
        }
        else {
            if (module.GetNumWarnings() > 0) {
                builder << "Compilation of shader file '" << file.filepath << "' exited with " << module.GetNumWarnings() << " warning(s)." << std::endl;
                context.warnings.emplace_back(builder.str());
            }

            context.spirv = { module.begin(), module.end() };
        }
    }

    std::vector<IDataType*> ShaderCompiler::EnumerateStructMembers(const spirv_cross::CompilerGLSL& compiler, const spirv_cross::SPIRType& structType) const {
        unsigned numMembers = structType.member_types.size();

        std::vector<IDataType*> members;
        members.resize(numMembers);

        for (unsigned memberIndex = 0; memberIndex < numMembers; ++memberIndex) {
            const spirv_cross::SPIRType& memberType = compiler.get_type(structType.member_types[memberIndex]);
            const std::string& memberName = compiler.get_member_name(structType.self, memberIndex);

            unsigned memberSize = compiler.get_declared_struct_member_size(structType, memberIndex);
            unsigned memberOffset = compiler.type_struct_member_offset(structType, memberIndex);

            // Struct member type has no distinction between declared and modified type.
            IDataType* type = ReflectDataType(compiler, memberType, memberType);

            if (IsMatrixType(type->dataType)) {

            }

            members[memberIndex] = ReflectDataType(compiler, memberType, memberType);

            std::cout << "found " << memberName << ", size: " << std::to_string(memberSize) << ", offset: " << std::to_string(memberOffset) << std::endl;
        }

        return members;
    }

    IDataType* ShaderCompiler::ReflectDataType(const spirv_cross::CompilerGLSL& compiler, const spirv_cross::SPIRType& declaredType, const spirv_cross::SPIRType& modifiedType) const {
        IDataType* type;

        unsigned numElements = declaredType.vecsize;
        unsigned numColumns = declaredType.columns;

        switch (declaredType.basetype) {
            case spirv_cross::SPIRType::Boolean: {
                assert(numColumns == 1); // Matrix of bools (bmatn) is not valid.
                type = ConstructPrimitiveType<bool>(numElements);
                break;
            }
            case spirv_cross::SPIRType::Int: {
                assert(numColumns == 1); // Matrix of ints is valid (imatn), but underlying type is only allowed to be floating point.
                type = ConstructPrimitiveType<int>(numElements);
                break;
            }
            case spirv_cross::SPIRType::UInt: {
                assert(numColumns == 1); // Matrix of uints is valid (umatn), but underlying type is only allowed to be floating point.
                type = ConstructPrimitiveType<unsigned>(numElements);
                break;
            }
            case spirv_cross::SPIRType::Float: {
                type = ConstructPrimitiveType<float>(numElements, numColumns);
                break;
            }
            case spirv_cross::SPIRType::Double: {
                type = ConstructPrimitiveType<double>(numElements, numColumns);
                break;
            }
            case spirv_cross::SPIRType::Struct: {
                type = new StructType(EnumerateStructMembers(compiler, declaredType));
                break;
            }
            case spirv_cross::SPIRType::AtomicCounter: {
                break;
            }
            case spirv_cross::SPIRType::SampledImage: {
                // sampler2D
                break;
            }
                // Vulkan-specific.
            case spirv_cross::SPIRType::Image: {
                // texture2D
                break;
            }
            case spirv_cross::SPIRType::Sampler: {
                // sampler
                break;
            }
            case spirv_cross::SPIRType::AccelerationStructure: {
                // accelerationStructure
                break;
            }
            case spirv_cross::SPIRType::RayQuery: {
                //
                break;
            }
                // Unsupported (variable) types in GLSL.
            case spirv_cross::SPIRType::Unknown:
            case spirv_cross::SPIRType::Void:
            case spirv_cross::SPIRType::SByte:  // signed char
            case spirv_cross::SPIRType::UByte:  // unsigned char
            case spirv_cross::SPIRType::Short:  // signed short
            case spirv_cross::SPIRType::UShort: // unsigned short
            case spirv_cross::SPIRType::Int64:  // signed long long
            case spirv_cross::SPIRType::UInt64: // unsigned long long
            case spirv_cross::SPIRType::Half:   // half
                // Internal types.
            case spirv_cross::SPIRType::ControlPointArray:
            case spirv_cross::SPIRType::Interpolant:
            case spirv_cross::SPIRType::Char:
                break; // TODO;
        }

        // Check decorators on modified type (arrays, pointers).
        if (!modifiedType.array.empty()) {
            // Array declaration of type[4][6] is declared as array = { 6, 4 }, with array_size_literal = { true, true }.
            for (int i = 0; i < modifiedType.array.size(); ++i) {
                unsigned dimension = modifiedType.array[i];
                bool literal = modifiedType.array_size_literal[i];

                // Stack array types within one another.
                type = new ArrayType(type, dimension);
            }
        }

        return type;
    }

}