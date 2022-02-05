
#include "common/geometry/mesh.h"
#include "common/api/backend.h"
#include "common/utility/imgui_log.h"

namespace Sandbox {

    Mesh::Mesh() : vao_(),
                   isDirty_(false),
                   topology_(MeshTopology::TRIANGLES),
                   vertexData_()
                   {
        // Mesh has one global data layout that does not change.
        vao_.Bind();

        // Vertex position.
        {
            BufferLayout bufferLayout { };
            bufferLayout.SetBufferElements( { BufferElement { ShaderDataType::VEC3, "vertexPosition" } } );
            vao_.AddVBO("position", bufferLayout);
        }

        // Vertex normal.
        {
            BufferLayout bufferLayout { };
            bufferLayout.SetBufferElements( { BufferElement { ShaderDataType::VEC3, "vertexNormal" } } );
            vao_.AddVBO("normal", bufferLayout);
        }

        // Vertex UV coordinates.
        {
            BufferLayout bufferLayout { };
            bufferLayout.SetBufferElements( { BufferElement { ShaderDataType::VEC2, "vertexUV" } } );
            vao_.AddVBO("uv", bufferLayout);
        }

        // TODO: TBN frame, skinned models.

        vao_.Unbind();
    }

    Mesh::~Mesh() {
    }

    Mesh::Mesh(const Mesh& other) : vao_(other.vao_),
                                    isDirty_(true),  // Buffers need updating.
                                    topology_(other.topology_),
                                    vertexData_(other.vertexData_),
                                    indices_(other.indices_)
                                    {
    }

    Mesh &Mesh::operator=(const Mesh &other) {
        if (this == &other) {
            return *this;
        }

        vao_ = other.vao_;
        isDirty_ = true;  // Buffers need updating.
        topology_ = other.topology_;
        vertexData_ = other.vertexData_;
        indices_ = other.indices_;

        return *this;
    }

    void Mesh::Bind() const {
        // TODO: smart bind.
        vao_.Bind();
    }

    void Mesh::Unbind() const {
        vao_.Unbind();
    }

    void Mesh::Render() const {
        // Indices array always has the most up-to-date index count.
        Backend::Rendering::DrawIndexed(GetRenderingPrimitive(topology_), static_cast<int>(indices_.size()));
    }

    void Mesh::Complete() {
        if (isDirty_) {
            // Configure mesh buffer data.
            // Vertex positions.
            {
                VertexBufferObject* vbo = vao_.GetVBO("position");
                assert(vbo);

                std::vector<glm::vec3> vertices = GetVertices();
                assert(!vertices.empty());

                vbo->SetData(vertices.size() * sizeof(vertices[0]), vertices.data());
            }

            // Vertex normals.
            {
                VertexBufferObject* vbo = vao_.GetVBO("normal");
                assert(vbo);

                std::vector<glm::vec3> normals = GetNormals();
                if (normals.empty()) {
                    // Recalculating normals requires vertices to be present, which was validated above.
                    RecalculateNormals();
                }

                vbo->SetData(normals.size() * sizeof(normals[0]), normals.data());
            }

            // Texture coordinates.
            {
                VertexBufferObject* vbo = vao_.GetVBO("uv");
                assert(vbo);

                std::vector<glm::vec2> uv = GetUVs();
                assert(!uv.empty());

                vbo->SetData(uv.size() * sizeof(uv[0]), uv.data());
            }

            // Indices.
            {
                ElementBufferObject* ebo = vao_.GetEBO();
                assert(ebo);

                // Indices were not set, configure them by default based on mesh topology.
                if (indices_.empty()) {
                    std::vector<glm::vec3> vertices = GetVertices();
                    assert(!vertices.empty());

                    std::size_t numVertices = vertices.size();
                    for (unsigned i = 0; i < numVertices; ++i) {
                        indices_.emplace_back(i);
                    }
                }

                ebo->SetData(indices_.size() * sizeof(indices_[0]), indices_.data());
            }

            isDirty_ = false;
        }
    }

    void Mesh::RecalculateNormals() {
        // Recalculate all vertex normals.
        std::vector<glm::vec3> vertices = GetVertices();

        // Calculate unique contributing vertex normals, per vertex.
        // (vertex -> contributing face normals at that vertex)
        std::unordered_map<glm::vec3, std::unordered_set<glm::vec3>> contributingFaceNormals;
        for (int i = 0; i < indices_.size(); i += 3) {
            const glm::vec3& v1 = vertices[indices_[i + 0]];
            const glm::vec3& v2 = vertices[indices_[i + 1]];
            const glm::vec3& v3 = vertices[indices_[i + 2]];

            const glm::vec3& faceNormal = glm::normalize(glm::cross(v3 - v2, v1 - v2));

            // Contribute face normal to vertex normal calculation.
            contributingFaceNormals[v1].emplace(faceNormal);
            contributingFaceNormals[v2].emplace(faceNormal);
            contributingFaceNormals[v3].emplace(faceNormal);
        }

        // Compute vertex normals.
        std::vector<glm::vec3> normals;
        normals.reserve(vertices.size()); // 1 to 1 mapping.

        for (const glm::vec3& vertex : vertices) {
            auto iterator = contributingFaceNormals.find(vertex);
            assert(iterator != contributingFaceNormals.end());

            // Vertex normals are computed by averaging the contributing face normals at each vertex.
            glm::vec3 normal(0.0f);
            for (const glm::vec3& n : iterator->second) {
                normal += n;
            }

            normals.emplace_back(glm::normalize(normal));
        }

        SetNormals(normals);
    }

    void Mesh::SetVertices(const std::vector<glm::vec3>& vertices) {
        int numVertices = vertices.size();
        vertexData_.resize(numVertices);

        for (int i = 0; i < numVertices; ++i) {
            vertexData_[i].vertex_ = vertices[i];
        }

        isDirty_ = true;
    }

    void Mesh::SetIndices(const std::vector<unsigned int>& indices, MeshTopology topology) {
        indices_ = indices;
        topology_ = topology;
        isDirty_ = true;
    }

    void Mesh::SetUVs(const std::vector<glm::vec2>& uv) {
        // No resizing for UV coordinates.
        unsigned limit = std::min(vertexData_.size(), uv.size());

        for (unsigned i = 0; i < limit; ++i) {
            vertexData_[i].uv_ = uv[i];
        }

        isDirty_ = true;
    }

    void Mesh::SetNormals(const std::vector<glm::vec3>& normals) {
        // No resizing for vertex normals.
        unsigned limit = std::min(vertexData_.size(), normals.size());

        for (unsigned i = 0; i < limit; ++i) {
            vertexData_[i].normal_ = normals[i];
        }

        isDirty_ = true;
    }

    std::vector<glm::vec3> Mesh::GetVertices() const {
        std::vector<glm::vec3> vertices;
        vertices.reserve(vertexData_.size());

        for (const Vertex& vertex : vertexData_) {
            vertices.emplace_back(vertex.vertex_);
        }

        return std::move(vertices);
    }

    std::vector<unsigned> Mesh::GetIndices() const {
        return indices_;
    }

    std::vector<glm::vec2> Mesh::GetUVs() const {
        std::vector<glm::vec2> uv;
        uv.reserve(vertexData_.size());

        for (const Vertex& vertex : vertexData_) {
            uv.emplace_back(vertex.uv_);
        }

        return std::move(uv);
    }

    std::vector<glm::vec3> Mesh::GetNormals() const {
        std::vector<glm::vec3> normals;
        normals.reserve(vertexData_.size());

        for (const Vertex& vertex : vertexData_) {
            normals.emplace_back(vertex.normal_);
        }

        return std::move(normals);
    }

}
