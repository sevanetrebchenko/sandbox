
#ifndef SANDBOX_FBO_H
#define SANDBOX_FBO_H

#include <sandbox_pch.h>
#include <framework/buffer/rbo.h>
#include <framework/texture.h>

namespace Sandbox {

    class FrameBufferObject {
        public:
            enum RenderTargetType {
                RGBA,
                DEPTH
            };

            FrameBufferObject(unsigned contentWidth, unsigned contentHeight);
            ~FrameBufferObject();

            void BindForReadWrite() const;
            void BindForRead() const;
            void BindForWrite() const;

            void Unbind() const;

            void DrawBuffers(int startingRenderTargetID = 0, int numRenderTargets = -1) const;
            void CopyDepthBufferTo(FrameBufferObject* other) const;

            void AttachRenderTarget(Texture* frameBufferTexture);
            void AttachDepthBuffer(RenderBufferObject* rbo);

            Texture* GetNamedRenderTarget(const std::string& textureBufferName) const;
            RenderBufferObject* GetDepthBuffer() const;

            bool CheckStatus() const;
            void SaveRenderTargetsToDirectory(const std::string& directoryPath) const;

            [[nodiscard]] unsigned GetWidth() const;
            [[nodiscard]] unsigned GetHeight() const;

        private:
            unsigned _contentWidth;
            unsigned _contentHeight;

            std::unordered_map<std::string, Texture*> _renderTargets;
            bool _hasDepthRenderTarget;
            RenderBufferObject* _depthBuffer;

            std::vector<GLuint> _drawBuffers;

            unsigned _currentColorAttachmentID;
            unsigned _bufferID;
    };

    void CopyDepthBuffer(FrameBufferObject* source, FrameBufferObject* destination);

}

#endif //SANDBOX_FBO_H
