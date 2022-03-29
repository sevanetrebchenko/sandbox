
#include "common/api/backend.h"
#include "common/geometry/mesh.h"
#include "common/api/buffer/vao_manager.h"

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

            void WriteDepth(bool mask) {
                mask ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
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

            glm::vec4 GetViewport() {
                glm::vec4 data;
                glGetFloatv(GL_VIEWPORT, &data[0]);
                return data;
            }
        }

        namespace Rendering {
            void DrawFSQ() {
                static bool initialized = false;
                static Mesh quad { VAOManager::Instance().GetVAO("assets/models/quad.obj") };
                if (!initialized) {
                    std::vector<glm::vec3> vertices;
                    std::vector<unsigned> indices;
                    std::vector<glm::vec2> uv;

                    vertices.emplace_back(-1.0f, 1.0f, 0.0f);
                    vertices.emplace_back(-1.0f, -1.0f, 0.0f);
                    vertices.emplace_back(1.0f, -1.0f, 0.0f);
                    vertices.emplace_back(1.0f, 1.0f, 0.0f);

                    indices.emplace_back(0);
                    indices.emplace_back(1);
                    indices.emplace_back(2);
                    indices.emplace_back(0);
                    indices.emplace_back(2);
                    indices.emplace_back(3);

                    uv.emplace_back(0.0f, 1.0f);
                    uv.emplace_back(0.0f, 0.0f);
                    uv.emplace_back(1.0f, 0.0f);
                    uv.emplace_back(1.0f, 1.0f);

                    quad.SetVertices(vertices);
                    quad.SetIndices(indices, MeshTopology::TRIANGLES);
                    quad.SetUVs(uv);
                    quad.RecalculateNormals();

                    quad.Complete();

                    initialized = true;
                }

                quad.Bind();
                quad.Render();
                quad.Unbind();
            }

            void DrawIndexed(GLuint renderingPrimitive, int indexCount) {
                glDrawElements(renderingPrimitive, indexCount, GL_UNSIGNED_INT, nullptr);
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