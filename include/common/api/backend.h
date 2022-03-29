
#ifndef SANDBOX_BACKEND_H
#define SANDBOX_BACKEND_H

#include "common/api/buffer/vao.h"
#include "common/api/shader/shader.h"

namespace Sandbox {

    namespace Backend {
        namespace Core {
            void SetViewport(unsigned x, unsigned y, unsigned width, unsigned height);

            // Returns (x, y) window coordinates of the viewport, followed by viewport (width, height).
            // TODO: abstract?
            [[nodiscard]] glm::vec4 GetViewport();

            void EnableFlag(GLuint flag);
            void DisableFlag(GLuint flag);

            void CullFace(GLuint face);

            void WriteDepth(bool mask);

            void ClearColor(float r, float g, float b, float a);
            void ClearFlag(GLuint flags);
        }

        namespace Rendering {
            void DrawFSQ();
            void DrawIndexed(GLuint renderingPrimitive, int indexCount);

            void ActivateTextureSampler(int samplerID);
            void BindTextureWithSampler(Shader* shader, Texture* texture, int samplerID);
            void BindTextureWithSampler(Shader* shader, Texture* texture, const std::string& samplerName, int samplerID); // If sampler name is different than texture name.
        }

    }

}

#endif //SANDBOX_BACKEND_H
