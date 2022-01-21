
#include "common/api/buffer/vbo.h"

namespace Sandbox {

    VertexBufferObject::VertexBufferObject(const BufferLayout& bufferLayout) : _bufferLayout(bufferLayout) {
        glGenBuffers(1, &_bufferID);
    }

    VertexBufferObject::~VertexBufferObject() {
        glDeleteBuffers(1, &_bufferID);
    }

    void VertexBufferObject::Bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, _bufferID);
    }

    void VertexBufferObject::Unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBufferObject::SetData(unsigned dataSize, const void *dataBase) const {
        glBindBuffer(GL_ARRAY_BUFFER, _bufferID);
        glBufferData(GL_ARRAY_BUFFER, dataSize, dataBase, GL_STATIC_DRAW);
    }

    const BufferLayout &VertexBufferObject::GetBufferLayout() const {
        return _bufferLayout;
    }

}