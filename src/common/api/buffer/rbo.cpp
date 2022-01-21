
#include "common/api/buffer/rbo.h"

namespace Sandbox {

    RenderBufferObject::RenderBufferObject() {
        glGenRenderbuffers(1, &_bufferID);
    }

    RenderBufferObject::~RenderBufferObject() {
        glDeleteRenderbuffers(1, &_bufferID);
    }

    void RenderBufferObject::Bind() const {
        glBindRenderbuffer(GL_RENDERBUFFER, _bufferID);
    }

    void RenderBufferObject::Unbind() const {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    void RenderBufferObject::ReserveData(unsigned width, unsigned height) {
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    }

    GLuint RenderBufferObject::ID() const {
        return _bufferID;
    }

}
