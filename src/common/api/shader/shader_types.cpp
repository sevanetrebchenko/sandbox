
#include "common/api/shader/shader_types.h"
#include "common/utility/directory.h"

#include <numeric>

namespace Sandbox {

    u32 ShaderDataTypeSize(ShaderDataType shaderDataType) {
        switch (shaderDataType) {
            case ShaderDataType::INT:
            case ShaderDataType::UINT:
                return sizeof(int);
            case ShaderDataType::BOOL:
                return sizeof(bool);
            case ShaderDataType::FLOAT:
                return sizeof(float);
            case ShaderDataType::VEC2:
                return sizeof(glm::vec2);
            case ShaderDataType::IVEC2:
                return sizeof(glm::ivec2);
            case ShaderDataType::VEC3:
                return sizeof(glm::vec3);
            case ShaderDataType::UVEC3:
                return sizeof(glm::uvec3);
            case ShaderDataType::VEC4:
                return sizeof(glm::vec4);
            case ShaderDataType::UVEC4:
                return sizeof(glm::uvec4);
            case ShaderDataType::MAT4:
                return sizeof(glm::mat4);
            default:
                throw std::runtime_error("Unknown shader data type provided to ShaderDataTypeSize.");
        }
    }

    bool IsScalarType(DataType dataType) {
        switch (dataType) {
            case DataType::BOOL:
            case DataType::INT:
            case DataType::UINT:
            case DataType::FLOAT:
            case DataType::DOUBLE:
                return true;
            case DataType::BVEC2:
            case DataType::IVEC2:
            case DataType::UVEC2:
            case DataType::VEC2:
            case DataType::DVEC2:
            case DataType::BVEC3:
            case DataType::IVEC3:
            case DataType::UVEC3:
            case DataType::VEC3:
            case DataType::DVEC3:
            case DataType::BVEC4:
            case DataType::IVEC4:
            case DataType::UVEC4:
            case DataType::VEC4:
            case DataType::DVEC4:
            case DataType::MAT2X2:
            case DataType::MAT2X3:
            case DataType::MAT2X4:
            case DataType::MAT3X2:
            case DataType::MAT3X3:
            case DataType::MAT3X4:
            case DataType::MAT4X2:
            case DataType::MAT4X3:
            case DataType::MAT4X4:
            case DataType::DMAT2X2:
            case DataType::DMAT2X3:
            case DataType::DMAT2X4:
            case DataType::DMAT3X2:
            case DataType::DMAT3X3:
            case DataType::DMAT3X4:
            case DataType::DMAT4X2:
            case DataType::DMAT4X3:
            case DataType::DMAT4X4:
            case DataType::STRUCT:
            case DataType::ARRAY:
            case DataType::UBO:
            case DataType::SSBO:
            case DataType::SAMPLER2D:
                return false;
            case DataType::INVALID:
            default:
                throw std::runtime_error("Invalid value passed into IsScalarType(...).");
        }
    }

    bool IsVectorType(DataType dataType) {
        switch (dataType) {
            case DataType::BVEC2:
            case DataType::IVEC2:
            case DataType::UVEC2:
            case DataType::VEC2:
            case DataType::DVEC2:
            case DataType::BVEC3:
            case DataType::IVEC3:
            case DataType::UVEC3:
            case DataType::VEC3:
            case DataType::DVEC3:
            case DataType::BVEC4:
            case DataType::IVEC4:
            case DataType::UVEC4:
            case DataType::VEC4:
            case DataType::DVEC4:
                return true;
            case DataType::BOOL:
            case DataType::INT:
            case DataType::UINT:
            case DataType::FLOAT:
            case DataType::DOUBLE:
            case DataType::MAT2X2:
            case DataType::MAT2X3:
            case DataType::MAT2X4:
            case DataType::MAT3X2:
            case DataType::MAT3X3:
            case DataType::MAT3X4:
            case DataType::MAT4X2:
            case DataType::MAT4X3:
            case DataType::MAT4X4:
            case DataType::DMAT2X2:
            case DataType::DMAT2X3:
            case DataType::DMAT2X4:
            case DataType::DMAT3X2:
            case DataType::DMAT3X3:
            case DataType::DMAT3X4:
            case DataType::DMAT4X2:
            case DataType::DMAT4X3:
            case DataType::DMAT4X4:
            case DataType::STRUCT:
            case DataType::ARRAY:
            case DataType::UBO:
            case DataType::SSBO:
            case DataType::SAMPLER2D:
                return false;
            case DataType::INVALID:
            default:
                throw std::runtime_error("Invalid value passed into IsVectorType(...).");
        }
    }

    bool IsMatrixType(DataType dataType) {
        switch (dataType) {
            case DataType::MAT2X2:
            case DataType::MAT2X3:
            case DataType::MAT2X4:
            case DataType::MAT3X2:
            case DataType::MAT3X3:
            case DataType::MAT3X4:
            case DataType::MAT4X2:
            case DataType::MAT4X3:
            case DataType::MAT4X4:
            case DataType::DMAT2X2:
            case DataType::DMAT2X3:
            case DataType::DMAT2X4:
            case DataType::DMAT3X2:
            case DataType::DMAT3X3:
            case DataType::DMAT3X4:
            case DataType::DMAT4X2:
            case DataType::DMAT4X3:
            case DataType::DMAT4X4:
                return true;
            case DataType::BOOL:
            case DataType::INT:
            case DataType::UINT:
            case DataType::FLOAT:
            case DataType::DOUBLE:
            case DataType::BVEC2:
            case DataType::IVEC2:
            case DataType::UVEC2:
            case DataType::VEC2:
            case DataType::DVEC2:
            case DataType::BVEC3:
            case DataType::IVEC3:
            case DataType::UVEC3:
            case DataType::VEC3:
            case DataType::DVEC3:
            case DataType::BVEC4:
            case DataType::IVEC4:
            case DataType::UVEC4:
            case DataType::VEC4:
            case DataType::DVEC4:
            case DataType::STRUCT:
            case DataType::ARRAY:
            case DataType::UBO:
            case DataType::SSBO:
            case DataType::SAMPLER2D:
                return false;
            case DataType::INVALID:
            default:
                throw std::runtime_error("Invalid value passed into IsMatrixType(...).");
        }
    }


    IDataType::IDataType() : dataType(DataType::INVALID) {
    }

    IDataType::~IDataType() = default;


    StructType::StructType(std::vector<IDataType*> inMembers) : IDataType(),
                                                                members(std::move(inMembers))
                                                                {
        dataType = DataType::STRUCT;
    }

    StructType::~StructType() {
        for (IDataType* member : members) {
            delete member;
        }
    }

    ArrayType::ArrayType(IDataType* inType, u32 inNumElements) : IDataType(),
                                                                 elementType(inType),
                                                                 numElements(inNumElements)
                                                                 {
        dataType = DataType::ARRAY;
    }

    ArrayType::~ArrayType() {
        delete elementType;
    }

}

