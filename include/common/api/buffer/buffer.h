
#ifndef SANDBOX_BUFFER_H
#define SANDBOX_BUFFER_H

#include "pch.h"
#include "common/api/shader/shader_data_types.h"

namespace Sandbox {

    class BufferElement {
        public:
            BufferElement(ShaderDataType shaderDataType, std::string elementName);
            ~BufferElement() = default;

            [[nodiscard]] unsigned GetComponentCount() const;
            [[nodiscard]] const std::string& GetName() const;

            void SetName(const std::string& elementName);
            [[nodiscard]] const ShaderDataType& GetShaderDataType() const;
            void SetShaderDataType(ShaderDataType shaderDataType);
            [[nodiscard]] size_t GetBufferOffset() const;
            void SetBufferOffset(unsigned bufferOffset);

        protected:
            std::string _elementName;
            ShaderDataType _shaderDataType;
            unsigned _elementSize;
            unsigned _bufferOffset;
    };

    class BufferLayout {
        public:
            BufferLayout();
            ~BufferLayout() = default;

            void SetBufferElements(const std::initializer_list<BufferElement>& bufferElements);

            [[nodiscard]] const std::vector<BufferElement>& GetBufferElements() const;

            [[nodiscard]] unsigned GetStride() const;

            std::vector<BufferElement>::iterator begin();
            std::vector<BufferElement>::iterator end();
            std::vector<BufferElement>::const_iterator cbegin();
            std::vector<BufferElement>::const_iterator cend();

        protected:
            unsigned _stride;

        private:
            std::vector<BufferElement> _elementLayout;
    };



}

#endif //SANDBOX_BUFFER_H
