
#ifndef SANDBOX_MESH_H
#define SANDBOX_MESH_H

#include "pch.h"
#include "common/api/buffer/vao.h"
#include "common/ecs/component/component.h"

#include "common/geometry/mesh/topology.h"

namespace Sandbox {

    struct Vertex {
        glm::vec3 vertex_;
        glm::vec2 uv_;     // (-1.0, -1.0f) if model does not have any UV coordinates.

        // Components of the TBN matrix.
        glm::vec3 normal_; // Vertex normal.
        glm::vec3 tangent_;
        glm::vec3 bitangent_;

        // Unused for non-animated models.
        // Allows for more than the standard 4 bone influence per vertex for animated models.
        std::vector<unsigned> boneIDs_;
        std::vector<float> boneWeights_;
    };

    class Mesh : public IComponent {
        public:
            // Default mesh topology is Triangles.
            Mesh(VertexArrayObject* vao);
            ~Mesh() override;

            Mesh(const Mesh& other);
            Mesh& operator=(const Mesh& other);

            void Bind() const;
            void Unbind() const;

            // Assumes mesh is already bound.
            void Render();
            virtual void Complete();

            // Allows for manual construction of meshes.
            // Mesh is always rendered using indexed rendering.
            // Mesh data is clamped to the number of vertices in the mesh.
            void SetVertices(const std::vector<glm::vec3>& vertices);

            // Default vertex indexing makes triangles out of consecutive vertices.
            // Example: (0, 1, 2) (3, 4, 5) (6, 7, 8) ... for triangles, (0, 1) (2, 3) (4, 5) ... for lines, etc.
            void SetIndices(const std::vector<unsigned>& indices, MeshTopology topology);
            void SetUVs(const std::vector<glm::vec2>& uv);

            // Manually specify VERTEX normals (1 to 1 mapping with vertices). Any excess normals will be ignored.
            void SetNormals(const std::vector<glm::vec3>& normals);

            // Performance note: functions recompute desired quantities when called.
            [[nodiscard]] std::vector<glm::vec3> GetVertices() const;
            [[nodiscard]] std::vector<unsigned> GetIndices() const;

            [[nodiscard]] std::vector<glm::vec2> GetUVs() const;
            [[nodiscard]] std::vector<glm::vec3> GetNormals() const;

            // Recalculates vertex normals.
            void RecalculateNormals();

        private:
            VertexArrayObject* vao_;
            bool isDirty_;

            MeshTopology topology_;

            // Mesh data.
            std::vector<Vertex> vertexData_;
            std::vector<unsigned> indices_;
    };

}

#endif //SANDBOX_MESH_H
