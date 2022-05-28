
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


    enum class DataType {

    };

    struct IDataType {
        IDataType(std::string inName, u32 inSize);
        virtual ~IDataType() = 0;

        u32 size; // Total size (in bytes) of element.
        std::string name;
    };

    template <typename T>
    struct ScalarType : public IDataType {
        ScalarType(std::string inName);
        ~ScalarType() override;
    };

    template <typename T>
    struct VectorType : public IDataType {
        VectorType(std::string inName, u32 inNumElements);
        ~VectorType() override;

        u32 numElements;
    };

    struct MatrixType : public IDataType {
        MatrixType(std::string inName, glm::ivec2 inDimensions);
        ~MatrixType() override;

        glm::ivec2 dimensions;
    };

    struct StructType : public IDataType {
        public:
            StructType(std::string name, std::vector<IDataType*> inMembers);
            ~StructType() override;

            std::vector<IDataType*> members;

        private:
            [[nodiscard]] u32 CalculateSize(const std::vector<IDataType*>& inMembers) const;
    };

    struct ArrayType : public IDataType {
        ArrayType(std::string inName, IDataType* inType, u32 inNumElements);
        ~ArrayType() override;

        IDataType* type;
        u32 numElements;
    };

}

#include "common/api/shader/shader_types.tpp"
