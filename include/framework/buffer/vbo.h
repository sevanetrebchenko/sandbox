
#ifndef SANDBOX_VBO_H
#define SANDBOX_VBO_H

#include <framework/buffer/buffer.h>

namespace Sandbox {

    class VertexBufferObject {
        public:
            explicit VertexBufferObject(const BufferLayout& bufferLayout);
            ~VertexBufferObject();

            void Bind() const;
            void Unbind() const;

            void SetData(unsigned dataSize, const void *dataBase) const;
            [[nodiscard]] const BufferLayout& GetBufferLayout() const;

        private:
            unsigned _bufferID;
            BufferLayout _bufferLayout;
    };

}

#endif //SANDBOX_VBO_H
