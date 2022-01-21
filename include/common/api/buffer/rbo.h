
#ifndef SANDBOX_RBO_H
#define SANDBOX_RBO_H

#include "pch.h"

namespace Sandbox {

    class RenderBufferObject {
        public:
            RenderBufferObject();
            ~RenderBufferObject();

            void Bind() const;
            void Unbind() const;

            [[nodiscard]] GLuint ID() const;

            void ReserveData(unsigned width, unsigned height);

        private:
            GLuint _bufferID;
    };

}

#endif //SANDBOX        _RBO_H
