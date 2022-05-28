
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

    IDataType::IDataType(std::string inName, u32 inSize) : size(inSize),
                                                           name(std::move(inName))
                                                           {
    }

    IDataType::~IDataType() = default;


    MatrixType::MatrixType(std::string inName, glm::ivec2 inDimensions) : IDataType(std::move(inName), sizeof(f32) * inDimensions.x * inDimensions.y),
                                                                          dimensions(inDimensions)
                                                                          {
    }

    MatrixType::~MatrixType() = default;


    StructType::StructType(std::string inName, std::vector<IDataType*> inMembers) : IDataType(std::move(inName), CalculateSize(inMembers)),
                                                                                    members(std::move(inMembers))
                                                                                    {
    }

    StructType::~StructType() {
        for (IDataType* member : members) {
            delete member;
        }
    }

    u32 StructType::CalculateSize(const std::vector<IDataType*>& inMembers) const {
        return std::accumulate(inMembers.begin(), inMembers.end(), 0u, [](u32 sum, IDataType* type) -> u32 {
            return sum + type->size;
        });
    }

    ArrayType::ArrayType(std::string inName, IDataType* inType, u32 inNumElements) : IDataType(std::move(inName), inType->size * inNumElements),
                                                                                     type(inType),
                                                                                     numElements(inNumElements)
                                                                                     {
    }

    ArrayType::~ArrayType() {
        delete type;
    }

}

