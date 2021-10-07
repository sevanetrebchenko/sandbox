
#ifndef SANDBOX_MESH_H
#define SANDBOX_MESH_H

#include <sandbox_pch.h>
#include <framework/buffer/vao.h>

namespace Sandbox {

    class Mesh {
        public:
            explicit Mesh(GLuint renderingPrimitive = -1u);
            virtual ~Mesh();

            Mesh(const Mesh& mesh);
            Mesh& operator=(const Mesh& mesh);

            void Bind() const;
            void Unbind() const;

            void SetVertices(const std::vector<glm::vec3>& vertices);
            void SetNormals(const std::vector<glm::vec3>& normals);
            void SetUV(const std::vector<glm::vec2>& uvs);
            void SetIndices(const std::vector<unsigned>& indices);
            void RecalculateNormals();
            void Complete();

            [[nodiscard]] const std::vector<glm::vec3>& GetVertices() const;
            [[nodiscard]] const std::vector<glm::vec3>& GetNormals() const;
            [[nodiscard]] const std::vector<glm::vec2>& GetUV() const;
            [[nodiscard]] const std::vector<unsigned>& GetIndices() const;
            [[nodiscard]] GLuint GetRenderingPrimitive() const;
            [[nodiscard]] const VertexArrayObject* GetVAO() const;

        protected:
            virtual void InitializeBuffers();

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
