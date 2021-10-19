
#ifndef SANDBOX_EBO_H
#define SANDBOX_EBO_H

#include <sandbox_pch.h>

namespace Sandbox {

    class ElementBufferObject {
        public:
            ElementBufferObject();
            ~ElementBufferObject();

            void Bind() const;
            void Unbind() const;
            void SetData(unsigned dataSize, const void *dataBase);
            [[nodiscard]] unsigned GetIndexCount() const;

        private:
            unsigned _bufferID;
            unsigned _indexCount;
    };

}

#endif //SANDBOX_EBO_H
