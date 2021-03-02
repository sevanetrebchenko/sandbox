
#ifndef SANDBOX_UBO_H
#define SANDBOX_UBO_H

#include <sandbox_pch.h>
#include <framework/shader.h>
#include <framework/buffer/buffer.h>
#include <framework/shader_uniform.h>

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


    class UniformBlockLayout : public BufferLayout {
        public:
            UniformBlockLayout(unsigned numInitialElements, unsigned numElementsInIntermediateBlock);
            ~UniformBlockLayout();

            void SetBufferElements(const std::vector<UniformBufferElement>& bufferElements);
            [[nodiscard]] std::vector<UniformBufferElement>& GetBufferElements();

            [[nodiscard]] unsigned GetInitialOffsetInElements() const;
            [[nodiscard]] unsigned GetIntermediateOffsetInElements() const;

        private:
            std::vector<UniformBufferElement> _elementLayout;

            unsigned _initialOffsetInElements;
            unsigned _intermediateOffsetInElements;
    };


    class UniformBlock {
        public:
            UniformBlock(const UniformBlockLayout& uniformBlockLayout, unsigned bindingPoint);

            [[nodiscard]] unsigned GetBindingPoint() const;
            [[nodiscard]] unsigned GetBufferOffset() const;
            [[nodiscard]] unsigned GetBlockDataSize() const;
            [[nodiscard]] UniformBlockLayout* GetUniformBlockLayout();

        private:
            friend class UniformBufferObject;

            UniformBlockLayout _blockLayout;
            unsigned _bindingPoint;
            unsigned _bufferOffset;
            unsigned _blockPadding;
    };


    class UniformBufferObject {
        public:
            UniformBufferObject();
            ~UniformBufferObject();

            void Bind() const;
            void Unbind() const;

            void AddUniformBlock(UniformBlock uniformBlock);
            UniformBlock* GetUniformBlock(unsigned bindingPoint);

            void SetSubData(unsigned elementOffset, unsigned elementSize, const void* data) const;

        private:
            std::unordered_map<unsigned, UniformBlock> _bufferBlocks;
            unsigned _totalBufferSize;
            unsigned _bufferID;
    };

}

#endif //SANDBOX_UBO_H
