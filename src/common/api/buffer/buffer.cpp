
#include "common/api/buffer/buffer.h"

namespace Sandbox {

    BufferElement::BufferElement(ShaderDataType shaderDataType, std::string elementName) : _elementName(std::move(elementName)),
                                                                                           _shaderDataType(shaderDataType),
                                                                                           _elementSize(ShaderDataTypeSize(shaderDataType)),
                                                                                           _bufferOffset(0u) {
        // Element offset gets initialized when attached to a BufferLayout.
    }

    unsigned BufferElement::GetComponentCount() const {
        switch (_shaderDataType) {
            case ShaderDataType::BOOL:
            case ShaderDataType::INT:
            case ShaderDataType::FLOAT:
                return 1;
            case ShaderDataType::VEC2:
                return 2;
            case ShaderDataType::VEC3:
                return 3;
            case ShaderDataType::VEC4:
            case ShaderDataType::MAT4:
                return 4;
            default:
                throw std::runtime_error("Unknown shader data type provided to BufferElement::GetComponentCount.");
        }
    }

    const std::string &BufferElement::GetName() const {
        return _elementName;
    }

    void BufferElement::SetName(const std::string& elementName) {
        _elementName = elementName;
    }

    const ShaderDataType &BufferElement::GetShaderDataType() const {
        return _shaderDataType;
    }

    void BufferElement::SetShaderDataType(ShaderDataType shaderDataType) {
        _shaderDataType = shaderDataType;
    }

    size_t BufferElement::GetBufferOffset() const {
        return _bufferOffset;
    }

    void BufferElement::SetBufferOffset(unsigned int bufferOffset) {
        _bufferOffset = bufferOffset;
    }



    BufferLayout::BufferLayout() : _stride(0) {
    }

    void BufferLayout::SetBufferElements(const std::initializer_list<BufferElement> &bufferElements) {
        _elementLayout = bufferElements;

        unsigned currentOffset = 0;
        for (auto& bufferElement : _elementLayout) {
            // Initialize offsets for buffer elements.
            bufferElement.SetBufferOffset(currentOffset);

            unsigned elementSize = ShaderDataTypeSize(bufferElement.GetShaderDataType());
            currentOffset += elementSize;
        }

        _stride = currentOffset;
    }

    const std::vector<BufferElement> &BufferLayout::GetBufferElements() const {
        return _elementLayout;
    }

    unsigned BufferLayout::GetStride() const {
        return _stride;
    }

    std::vector<BufferElement>::iterator BufferLayout::begin() {
        return _elementLayout.begin();
    }

    std::vector<BufferElement>::iterator BufferLayout::end() {
        return _elementLayout.end();
    }

    std::vector<BufferElement>::const_iterator BufferLayout::cbegin() {
        return _elementLayout.cbegin();
    }

    std::vector<BufferElement>::const_iterator BufferLayout::cend() {
        return _elementLayout.cend();
    }

}
