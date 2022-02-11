
#ifndef SANDBOX_UBO_H
#define SANDBOX_UBO_H

#include "pch.h"
#include "common/api/shader/shader_types.h"
#include "common/api/buffer/buffer.h"
#include "common/api/shader/shader_uniform.h"

namespace Sandbox {

    class UniformBufferElement : public BufferElement {
        public:
            static unsigned UBOShaderDataTypeSize(ShaderDataType dataType);

            UniformBufferElement(ShaderDataType shaderDataType, std::string elementName);
            ~UniformBufferElement();

            [[nodiscard]] bool IsEnabled() const;
            void SetEnabled(bool enabled);

        private:
            bool _enabled;
            ShaderUniform::UniformEntry _data;
    };


    class UniformBlockLayout {
        public:
            UniformBlockLayout();
            ~UniformBlockLayout();

            void SetBufferElements(unsigned numInitialElements, unsigned numElementsInIntermediateBlock, const std::vector<UniformBufferElement>& bufferElements);
            void SetBufferElements(unsigned numInitialElements, unsigned numElementsInIntermediateBlock, const std::initializer_list<UniformBufferElement>& bufferElements);
            [[nodiscard]] const std::vector<UniformBufferElement>& GetBufferElements() const;

            [[nodiscard]] unsigned GetInitialOffsetInElements() const;
            [[nodiscard]] unsigned GetIntermediateOffsetInElements() const;
            [[nodiscard]] unsigned GetStride() const;

        private:
            std::vector<UniformBufferElement> _elementLayout;

            unsigned _stride;
            unsigned _initialOffsetInElements;
            unsigned _intermediateOffsetInElements;
    };


    class UniformBlock {
        public:
            UniformBlock(unsigned bindingPoint, const UniformBlockLayout& uniformBlockLayout);
            UniformBlock(UniformBlock&& other) noexcept;

            [[nodiscard]] unsigned GetBindingPoint() const;
            [[nodiscard]] unsigned GetBlockDataSize() const;
            [[nodiscard]] UniformBlockLayout& GetUniformBlockLayout();

        private:
            friend class UniformBufferObject;

            UniformBlockLayout _blockLayout;
            unsigned _bindingPoint;
    };


    class UniformBufferObject {
        public:
            UniformBufferObject();
            ~UniformBufferObject();

            void Bind() const;
            void Unbind() const;

            void SetUniformBlock(UniformBlock& uniformBlock);
            UniformBlock& GetUniformBlock();
            void SetSubData(unsigned elementOffset, unsigned elementSize, const void* data) const;

        private:
            UniformBlock* _uniformBlock;
            unsigned _bufferID;
    };

}

#endif //SANDBOX_UBO_H
