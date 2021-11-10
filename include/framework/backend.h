
#ifndef SANDBOX_BACKEND_H
#define SANDBOX_BACKEND_H

#include <framework/buffer/buffer.h>
#include <framework/shader.h>
#include <framework/texture.h>
#include <framework/buffer/vao.h>

namespace Sandbox {

    namespace Backend {
        namespace Core {
            void SetViewport(unsigned x, unsigned y, unsigned width, unsigned height);

            void EnableFlag(GLuint flag);
            void DisableFlag(GLuint flag);

            void CullFace(GLuint face);

            void WriteDepth(bool mask);

            void ClearColor(float r, float g, float b, float a);
            void ClearColor(glm::vec4 color);

            void ClearFlag(GLuint flags);
        }

        namespace Rendering {
            void DrawFSQ();
            void DrawIndexed(const VertexArrayObject* vao, GLuint renderingPrimitive);

            void ActivateTextureSampler(int samplerID);
            void BindTextureWithSampler(Shader* shader, Texture* texture, int samplerID);
            void BindTextureWithSampler(Shader* shader, Texture* texture, const std::string& samplerName, int samplerID); // If sampler name is different than texture name.
        }

    }

}

#endif //SANDBOX_BACKEND_H
