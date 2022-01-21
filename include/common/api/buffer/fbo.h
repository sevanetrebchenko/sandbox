
#ifndef SANDBOX_FBO_H
#define SANDBOX_FBO_H

#include "pch.h"
#include "common/api/buffer/rbo.h"
#include "common/texture/texture.h"

namespace Sandbox {

    class FrameBufferObject {
        public:
            enum RenderTargetType {
                RGBA,
                DEPTH
            };

            FrameBufferObject(int contentWidth, int contentHeight);
            ~FrameBufferObject();

            // Reallocates storage for FBO render attachments and depth buffer to match input dimensions.
            void Reallocate(int contentWidth, int contentHeight);
            void Reallocate(glm::ivec2 contentDimensions);

            void BindForReadWrite() const;
            void BindForRead() const;
            void BindForWrite() const;

            void Unbind() const;

            void DrawBuffers(int startingRenderTargetID = 0, int numRenderTargets = -1) const;
            void CopyDepthBufferTo(FrameBufferObject* other) const;

            void AttachRenderTarget(Texture* frameBufferTexture);
            void AttachDepthBuffer(RenderBufferObject* rbo);

            [[nodiscard]] Texture* GetNamedRenderTarget(const std::string& textureBufferName) const;
            [[nodiscard]] RenderBufferObject* GetDepthBuffer() const;

            [[nodiscard]] bool CheckStatus() const;

            // Expects directory path with appended separator (/).
            void SaveRenderTargetsToDirectory(const std::string& directoryPath) const;

            [[nodiscard]] unsigned GetWidth() const;
            [[nodiscard]] unsigned GetHeight() const;

        private:
            void RegenerateBufferID();

            int _contentWidth;
            int _contentHeight;

            std::unordered_map<std::string, Texture*> _renderTargetsMap;
            std::vector<Texture*> _renderTargetsList;
            bool _hasDepthRenderTarget;
            RenderBufferObject* _depthBuffer;

            std::vector<GLenum> _drawBuffers;

            unsigned _currentColorAttachmentID;
            unsigned _bufferID;
    };

    void CopyDepthBuffer(FrameBufferObject* source, FrameBufferObject* destination);

}

#endif //SANDBOX_FBO_H
