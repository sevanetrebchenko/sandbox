
#pragma once

#include "pch.h"
#include "common/utility/defines.h"

namespace Sandbox {

    enum class ShaderDataType {
        NONE,
        BOOL, INT, UINT, FLOAT,
        VEC2, IVEC2, VEC3, UVEC3, VEC4, UVEC4,
        MAT4,
    };

    u32 ShaderDataTypeSize(ShaderDataType shaderDataType);


    enum class DataType : unsigned {
        INVALID = 0,

        // Valid scalar types.
        BOOL,
        INT, UINT,
        FLOAT, DOUBLE,

        // Valid vector types.
        BVEC2, IVEC2, UVEC2, VEC2, DVEC2,
        BVEC3, IVEC3, UVEC3, VEC3, DVEC3,
        BVEC4, IVEC4, UVEC4, VEC4, DVEC4,

        // Valid matrix types.
        MAT2X2, MAT2X3, MAT2X4, MAT3X2, MAT3X3, MAT3X4, MAT4X2, MAT4X3, MAT4X4,
        DMAT2X2, DMAT2X3, DMAT2X4, DMAT3X2, DMAT3X3, DMAT3X4, DMAT4X2, DMAT4X3, DMAT4X4,

        // Valid aggregate types.
        STRUCT,
        ARRAY,
        UBO, SSBO,
        SAMPLER2D,
    };

    [[nodiscard]] bool IsScalarType(DataType dataType);
    [[nodiscard]] bool IsVectorType(DataType dataType);
    [[nodiscard]] bool IsMatrixType(DataType dataType);

    struct IDataType {
        IDataType();
        virtual ~IDataType() = 0;

        DataType dataType;
    };

    template <typename T>
    struct ScalarType : public IDataType {
        ScalarType();
        ~ScalarType() override;
    };

    template <typename T>
    struct VectorType : public IDataType {
        VectorType(unsigned inNumElements);
        ~VectorType() override;

        unsigned numElements;
    };

    template <typename T>
    struct MatrixType : public IDataType {
        MatrixType(unsigned inNumRows, unsigned inNumColumns);
        ~MatrixType() override;

        unsigned numRows;
        unsigned numColumns;
    };

    struct StructType : public IDataType {
        StructType(std::vector<IDataType*> inMembers);
        ~StructType() override;

        std::vector<IDataType*> members;
    };

    struct ArrayType : public IDataType {
        ArrayType(IDataType* inType, unsigned inNumElements);
        ~ArrayType() override;

        IDataType* elementType;
        unsigned numElements;
    };

    struct UniformBlockType : public IDataType {
        UniformBlockType(std::vector<IDataType*> inMembers);
        ~UniformBlockType() override;

        std::vector<IDataType*> members;
    };

    struct ShaderStorageBlockType : public IDataType {
    };

    struct SamplerType : public IDataType {
    };

}

#include "common/api/shader/shader_types.tpp"
