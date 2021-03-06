
#include <framework/buffer/fbo.h>
#include <framework/directory_utils.h>

namespace Sandbox {

    FrameBufferObject::FrameBufferObject(unsigned contextWidth, unsigned contextHeight) : _contentWidth(contextWidth),
                                                                                          _contentHeight(contextHeight),
                                                                                          _depthBuffer(nullptr),
                                                                                          _currentColorAttachmentID(0),
                                                                                          _hasDepthRenderTarget(false) {
        glGenFramebuffers(1, &_bufferID);
    }

    FrameBufferObject::~FrameBufferObject() {
        BindForReadWrite();

        for (auto& objectPair : _renderTargetsMap) {
            delete objectPair.second;
        }

        delete _depthBuffer;

        glDeleteFramebuffers(1, &_bufferID);
    }

    void FrameBufferObject::BindForReadWrite() const {
        glBindFramebuffer(GL_FRAMEBUFFER, _bufferID);
    }

    void FrameBufferObject::BindForRead() const {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _bufferID);
    }

    void FrameBufferObject::BindForWrite() const {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _bufferID);
    }

    void FrameBufferObject::Unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBufferObject::DrawBuffers(int startingRenderTargetID, int numRenderTargets) const {
        if (numRenderTargets == -1) {
            // Draw to all buffers.
            glDrawBuffers(_drawBuffers.size(), _drawBuffers.data());
        }
        else {
            // Draw to a subset of buffers.
            std::vector<GLenum> subDrawBuffers(_drawBuffers.begin() + startingRenderTargetID, _drawBuffers.begin() + startingRenderTargetID + numRenderTargets);
            glDrawBuffers(numRenderTargets, subDrawBuffers.data());
        }
    }

    void FrameBufferObject::CopyDepthBufferTo(FrameBufferObject* other) const {
        // Reading from this FBO.
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _bufferID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, other->_bufferID);

        // Copy over depth information.
        glBlitFramebuffer(0, 0, _contentWidth, _contentHeight, 0, 0, other->_contentWidth, other->_contentHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    void FrameBufferObject::AttachRenderTarget(Texture* renderTarget) {
        if (renderTarget) {
            GLenum attachment;

            switch (renderTarget->GetAttachmentType()) {
                case Texture::AttachmentType::COLOR:
                    attachment = GL_COLOR_ATTACHMENT0 + _currentColorAttachmentID;
                    break;
                case Texture::AttachmentType::DEPTH:
                    attachment = GL_DEPTH_ATTACHMENT;
                    _hasDepthRenderTarget = true;
                    break;
                case Texture::UNKNOWN:
                    throw std::runtime_error("Attempting to attach texture allocated with STB as a render target.");
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, renderTarget->ID(), 0);

            _renderTargetsMap.emplace(renderTarget->GetName(), renderTarget);
            _renderTargetsList.emplace_back(renderTarget);
            _drawBuffers.emplace_back(attachment);
            renderTarget->SetAttachmentLocation(_currentColorAttachmentID);

            ++_currentColorAttachmentID;
        }
    }

    void FrameBufferObject::AttachDepthBuffer(RenderBufferObject *rbo) {
        if (rbo) {
            if (!_hasDepthRenderTarget) {
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo->ID());
                _depthBuffer = rbo;
            }
            else {
                std::cout << "Framebuffer already has depth attachment in the form of a render target." << std::endl;
            }
        }
    }

    Texture *FrameBufferObject::GetNamedRenderTarget(const std::string &textureBufferName) const {
        auto textureBufferIter = _renderTargetsMap.find(textureBufferName);

        if (textureBufferIter != _renderTargetsMap.end()) {
            return textureBufferIter->second;
        }

        return nullptr;
    }

    RenderBufferObject* FrameBufferObject::GetDepthBuffer() const {
        return _depthBuffer;
    }

    bool FrameBufferObject::CheckStatus() const {
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }

    unsigned FrameBufferObject::GetWidth() const {
        return _contentWidth;
    }

    unsigned FrameBufferObject::GetHeight() const {
        return _contentHeight;
    }

    void FrameBufferObject::SaveRenderTargetsToDirectory(const std::string &directoryPath) const {
        std::string appendedDirectory = NativePathConverter::ConvertToNativeSeparators(directoryPath + "screenshots/");

        for (const auto& renderTarget : _renderTargetsMap) {
            std::string name = renderTarget.first;
            Texture* texture = renderTarget.second;

            texture->WriteDataToDirectory(appendedDirectory);
        }
    }

    void CopyDepthBuffer(FrameBufferObject* source, FrameBufferObject* destination) {
        // Keep track of the current bound framebuffer(s).
        GLint currentBoundReadFBO, currentBoundDrawFBO;
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &currentBoundReadFBO);
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentBoundDrawFBO);

        source->CopyDepthBufferTo(destination);

        // Restore state.
        glBindFramebuffer(GL_READ_FRAMEBUFFER, currentBoundReadFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, currentBoundDrawFBO);
    }

}
