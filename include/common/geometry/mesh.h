
#ifndef SANDBOX_MESH_H
#define SANDBOX_MESH_H

#include "pch.h"
#include "common/api/buffer/vao.h"
#include "common/ecs/component/component.h"

namespace Sandbox {

    class Mesh : public IComponent {
        public:
            explicit Mesh(GLuint renderingPrimitive = -1u);
            ~Mesh() override;

            Mesh(const Mesh& mesh);
            Mesh& operator=(const Mesh& mesh);

            void Bind() const;
            void Unbind() const;

            void SetVertices(const std::vector<glm::vec3>& vertices);
            void SetUV(const std::vector<glm::vec2>& uvs);
            void SetIndices(const std::vector<unsigned>& indices);
            void RecalculateNormals();
            void Complete();

            [[nodiscard]] const std::vector<glm::vec3>& GetVertices() const;
            [[nodiscard]] const std::vector<glm::vec2>& GetUV() const;
            [[nodiscard]] const std::vector<unsigned>& GetIndices() const;
            [[nodiscard]] GLuint GetRenderingPrimitive() const;
            [[nodiscard]] const VertexArrayObject* GetVAO() const;

        private:
            void InitializeBuffers();

            VertexArrayObject* _vao;
            GLuint _renderingPrimitive;
            bool _isDirty;

            std::vector<glm::vec3> _vertices;
            std::vector<glm::vec2> _uv;
            std::vector<unsigned> _triangles;
            std::vector<glm::vec3> _normals;
    };

}

#endif //SANDBOX_MESH_H
