
#include "common/api/buffer/ebo.h"

namespace Sandbox {

    ElementBufferObject::ElementBufferObject() {
        glGenBuffers(1, &_bufferID);
    }

    ElementBufferObject::~ElementBufferObject() {
        glDeleteBuffers(1, &_bufferID);
    }

    void ElementBufferObject::Bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _bufferID);
    }

    void ElementBufferObject::Unbind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void ElementBufferObject::SetData(unsigned dataSize, const void *dataBase) {
        _indexCount = dataSize / sizeof(unsigned);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _bufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, dataBase, GL_STATIC_DRAW);
    }

    unsigned ElementBufferObject::GetIndexCount() const {
        return _indexCount;
    }

}