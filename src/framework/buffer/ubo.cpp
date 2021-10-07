
#include <framework/buffer/ubo.h>

namespace Sandbox {

    unsigned UniformBufferElement::UBOShaderDataTypeSize(ShaderDataType dataType) {
        switch (dataType) {
            case ShaderDataType::BOOL:
                return 1;
            case ShaderDataType::INT:
            case ShaderDataType::UINT:
            case ShaderDataType::FLOAT:
                return 4;
            case ShaderDataType::VEC2:
            case ShaderDataType::IVEC2:
                return 8;
            case ShaderDataType::VEC3:
            case ShaderDataType::UVEC3:
            case ShaderDataType::VEC4:
            case ShaderDataType::UVEC4:
            case ShaderDataType::MAT4:
                return 16;
            default:
                throw std::runtime_error("Unknown shader data type provided to UBOShaderDataTypeSize.");
        }
    }

    UniformBufferElement::UniformBufferElement(ShaderDataType shaderDataType, std::string elementName) : BufferElement(shaderDataType, std::move(elementName)),
                                                                                                         _enabled(false)
    {

    }

    UniformBufferElement::~UniformBufferElement() {

    }

    bool UniformBufferElement::IsEnabled() const {
        return _enabled;
    }

    void UniformBufferElement::SetEnabled(bool enabled) {
        _enabled = enabled;
    }



    UniformBlockLayout::UniformBlockLayout() : _initialOffsetInElements(0u),
                                               _intermediateOffsetInElements(0u),
                                               _stride(0) {
    }

    UniformBlockLayout::~UniformBlockLayout() {
    }

    void UniformBlockLayout::SetBufferElements(unsigned numInitialElements, unsigned numElementsInIntermediateBlock, const std::vector<UniformBufferElement> &bufferElements) {
        _initialOffsetInElements = numInitialElements;
        _intermediateOffsetInElements = numElementsInIntermediateBlock;

        // Construct indices.
        _elementLayout = bufferElements;
        unsigned currentOffset = 0;

        unsigned structElementCounter = 0;

        for (unsigned i = 0; i < _elementLayout.size(); ++i) {
            UniformBufferElement& bufferElement = _elementLayout[i];

            unsigned elementAlignment = 0;
            unsigned totalElementSize = 0;

            // Each variable has a base alignment equal to the space a variable takes.
            switch (bufferElement.GetShaderDataType()) {
                // Size of N (4).
                case ShaderDataType::BOOL:
                case ShaderDataType::INT:
                case ShaderDataType::UINT:
                case ShaderDataType::FLOAT:
                    elementAlignment = 4;
                    totalElementSize = UniformBufferElement::UBOShaderDataTypeSize(ShaderDataType::FLOAT);
                    break;
                    // Size of 2N (8).
                case ShaderDataType::VEC2:
                case ShaderDataType::IVEC2:
                    elementAlignment = 8;
                    totalElementSize = UniformBufferElement::UBOShaderDataTypeSize(ShaderDataType::VEC2);
                    break;
                    // Size of 4N (16).
                case ShaderDataType::VEC3:
                case ShaderDataType::IVEC3:
                case ShaderDataType::UVEC3:
                case ShaderDataType::VEC4:
                case ShaderDataType::IVEC4:
                case ShaderDataType::UVEC4:
                    elementAlignment = 16;
                    totalElementSize = UniformBufferElement::UBOShaderDataTypeSize(ShaderDataType::VEC4);
                    break;
                    // Size of 4N (16) per column.
                case ShaderDataType::MAT4:
                    elementAlignment = 16;
                    totalElementSize = UniformBufferElement::UBOShaderDataTypeSize(ShaderDataType::VEC4) * 4;
                    break;
                default:
                    break;
            }

            if (i > _initialOffsetInElements) {
                ++structElementCounter;

                // Reached boundary between intermediate block.
                if (structElementCounter % _intermediateOffsetInElements == 0 && currentOffset % 16 != 0) {
                    currentOffset += 16 - currentOffset % 16;
                }
            }

            // Add padding if the element base alignment doesn't fall on a multiple of the variable space.
            if (currentOffset % elementAlignment != 0) {
                unsigned paddingBytes = elementAlignment - currentOffset % elementAlignment;
                currentOffset += paddingBytes;
            }

            // Initialize offsets for buffer elements.
            bufferElement.SetBufferOffset(currentOffset);

            // Add the element size to the offset.
            currentOffset += totalElementSize;
        }

        _stride = currentOffset;
    }

    const std::vector<UniformBufferElement> &UniformBlockLayout::GetBufferElements() const {
        return _elementLayout;
    }

    unsigned UniformBlockLayout::GetInitialOffsetInElements() const {
        return _initialOffsetInElements;
    }

    unsigned UniformBlockLayout::GetIntermediateOffsetInElements() const {
        return _intermediateOffsetInElements;
    }

    unsigned UniformBlockLayout::GetStride() const {
        return _stride;
    }

    void UniformBlockLayout::SetBufferElements(unsigned int numInitialElements, unsigned int numElementsInIntermediateBlock, const std::initializer_list<UniformBufferElement> &bufferElements) {
        std::vector<UniformBufferElement> elements = bufferElements;
        SetBufferElements(numInitialElements, numElementsInIntermediateBlock, elements);
    }


    UniformBlock::UniformBlock(unsigned bindingPoint, const UniformBlockLayout& uniformBlockLayout) : _blockLayout(uniformBlockLayout),
                                                                                                      _bindingPoint(bindingPoint)
    {
    }

    unsigned UniformBlock::GetBindingPoint() const {
        return _bindingPoint;
    }

    unsigned UniformBlock::GetBlockDataSize() const {
        return _blockLayout.GetStride();
    }

    UniformBlockLayout& UniformBlock::GetUniformBlockLayout() {
        return _blockLayout;
    }

    UniformBlock::UniformBlock(UniformBlock &&other) noexcept : _bindingPoint(other._bindingPoint),
                                                                _blockLayout(other._blockLayout) {
    }


    UniformBufferObject::UniformBufferObject() : _uniformBlock(nullptr) {
        glGenBuffers(1, &_bufferID);
    }

    UniformBufferObject::~UniformBufferObject() {
        glDeleteBuffers(1, &_bufferID);
    }

    void UniformBufferObject::Bind() const {
        glBindBuffer(GL_UNIFORM_BUFFER, _bufferID);
    }

    void UniformBufferObject::Unbind() const {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    UniformBlock& UniformBufferObject::GetUniformBlock() {
        return *_uniformBlock;
    }

    void UniformBufferObject::SetSubData(unsigned int elementOffset, unsigned int elementSize, const void *data) const {
        glBufferSubData(GL_UNIFORM_BUFFER, elementOffset, elementSize, data);
    }

    void UniformBufferObject::SetUniformBlock(UniformBlock &uniformBlock) {
        _uniformBlock = new UniformBlock(std::move(uniformBlock));

        // Allocate space for the new block within the buffer.
        Bind();
        unsigned bindingPoint = _uniformBlock->GetBindingPoint();
        unsigned blockSize = _uniformBlock->GetBlockDataSize();
        glBufferData(GL_UNIFORM_BUFFER, blockSize, nullptr, GL_STATIC_DRAW);
        Unbind();

        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, _bufferID);
    }

}