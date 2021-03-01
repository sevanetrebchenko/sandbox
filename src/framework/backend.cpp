
#include <framework/backend.h>

namespace Sandbox {
    namespace Backend {

        namespace Core {
            void EnableFlag(GLuint flag) {
                glEnable(flag);
            }

            void DisableFlag(GLuint flag) {
                glDisable(flag);
            }

            void CullFace(GLuint face) {
                glCullFace(face);
            }

            void ClearColor(float r, float g, float b, float a) {
                glClearColor(r, g, b, a);
            }

            void ClearFlag(GLuint flag) {
                glClear(flag);
            }

            void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
                glViewport(x, y, width, height);
            }
        }

        namespace Rendering {
            void DrawIndexed(const VertexArrayObject *vao, GLuint renderingPrimitive) {
                if (vao && vao->GetEBO()) {
                    glDrawElements(renderingPrimitive, vao->GetEBO()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
                }
            }

            void ActivateTextureSampler(int samplerID) {
                glActiveTexture(GL_TEXTURE0 + samplerID);
            }

            void BindTextureWithSampler(Shader* shader, Texture* renderTarget, int samplerID) {
                ActivateTextureSampler(samplerID);
                shader->SetUniform(renderTarget->GetName(), samplerID);
                renderTarget->Bind();
            }

            void BindTextureWithSampler(Shader* shader, Texture* renderTarget, const std::string& samplerName, int samplerID) {
                ActivateTextureSampler(samplerID);
                shader->SetUniform(samplerName, samplerID);
                renderTarget->Bind();
            }
        }

    }

}