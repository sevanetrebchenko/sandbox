
#include <framework/mesh.h>

namespace Sandbox {

    Mesh::Mesh(GLuint renderingPrimitive) : _renderingPrimitive(renderingPrimitive),
                                            _vao(nullptr),
                                            _isDirty(true) {
    }

    Mesh::~Mesh() {
        delete _vao;
    }

    Mesh::Mesh(const Mesh &mesh) : _renderingPrimitive(mesh._renderingPrimitive),
                                   _vao(nullptr),
                                   _isDirty(true),
                                   _vertices(mesh._vertices),
                                   _uv(mesh._uv),
                                   _triangles(mesh._triangles),
                                   _normals(mesh._normals) {
    }

    Mesh &Mesh::operator=(const Mesh &mesh) {
        if (this == &mesh) {
            return *this;
        }

        _vao = nullptr; // Do not initialize buffers.
        _isDirty = true;
        _renderingPrimitive = mesh._renderingPrimitive;
        _vertices = mesh._vertices;
        _triangles = mesh._triangles;
        _uv = mesh._uv;
        _normals = mesh._normals;

        return *this;
    }

    void Mesh::RecalculateNormals() {
        std::size_t numIndices = _triangles.size();
        assert(numIndices % 3 == 0);

        std::vector<std::vector<glm::vec3>> vertexAdjacentFaceNormals;
        vertexAdjacentFaceNormals.resize(_vertices.size());

        // Traverse all triples of indices and compute face normals from each.
        // Attempt to add each normal to the involved vertex if it hasn't been already.
        for (int i = 0; i < numIndices; i += 3) {
            unsigned& index1 = _triangles[i + 0];
            unsigned& index2 = _triangles[i + 1];
            unsigned& index3 = _triangles[i + 2];
            glm::vec3& vertex1 = _vertices[index1];
            glm::vec3& vertex2 = _vertices[index2];
            glm::vec3& vertex3 = _vertices[index3];

            // Calculate face normal.
            glm::vec3 faceNormal = glm::cross(vertex3 - vertex2, vertex1 - vertex2);

            bool duplicateNormal = false;
            // Attempt to add each normal to the involved vertices.
            for (unsigned j = 0; j < 3; ++j) {
                unsigned& index = _triangles[i + j];
                // Check if normal was already added to this face's vertices.
                for (const auto &normal : vertexAdjacentFaceNormals[index]) {
                    if ((glm::dot(faceNormal, normal) - (faceNormal.x * faceNormal.x + faceNormal.y * faceNormal.y + faceNormal.z * faceNormal.z)) > std::numeric_limits<float>::epsilon()) {
                        duplicateNormal = true;
                        break;
                    }
                }

                if (!duplicateNormal) {
                    vertexAdjacentFaceNormals[index].emplace_back(faceNormal);
                }
            }
        }

        // Compute normals from precomputed adjacent normal list.
        int numNormals = vertexAdjacentFaceNormals.size();
        _normals.resize(numNormals);

        // Fill mesh data normal data buffer.
        for (int i = 0; i < numNormals; ++i) {
            glm::vec3& vertexNormal = _normals[i];

            // Sum all adjacent face normals (without duplicates).
            for (const glm::vec3& normal : vertexAdjacentFaceNormals[i]) {
                vertexNormal += normal;
            }

            vertexNormal = glm::normalize(vertexNormal);
        }
    }

    void Mesh::Bind() const {
        if (_vao) {
            _vao->Bind();
        }
    }

    void Mesh::Unbind() const {
        if (_vao) {
            _vao->Unbind();
        }
    }

    void Mesh::SetVertices(const std::vector<glm::vec3> &vertices) {
        _vertices = vertices;
    }

    void Mesh::SetUV(const std::vector<glm::vec2> &uvs) {
        _uv = uvs;
    }

    void Mesh::SetIndices(const std::vector<unsigned int>& indices) {
        _triangles = indices;
    }

    void Mesh::Complete() {
        if (_isDirty) {
            InitializeBuffers();
        }
    }

    const std::vector<glm::vec3> &Mesh::GetVertices() const {
        return _vertices;
    }

    const std::vector<glm::vec2> &Mesh::GetUV() const {
        return _uv;
    }

    const std::vector<unsigned> &Mesh::GetIndices() const {
        return _triangles;
    }

    GLuint Mesh::GetRenderingPrimitive() const {
        return _renderingPrimitive;
    }

    const VertexArrayObject *Mesh::GetVAO() const {
        return _vao;
    }

    void Mesh::InitializeBuffers() {
        if (_isDirty) {
            if (!_vao) {
                _vao = new VertexArrayObject();
            }

            BufferLayout vertexBufferLayout;
            vertexBufferLayout.SetBufferElements({ {ShaderDataType::VEC3, "vertexPosition"} });

            VertexBufferObject* vertices = new VertexBufferObject(vertexBufferLayout);
            vertices->SetData(_vertices.size() * sizeof(glm::vec3), _vertices.data());

            // Line meshes have no normals.
            bool hasNormals = !_normals.empty();
            VertexBufferObject* normals = nullptr;
            if (hasNormals) {
                BufferLayout normalBufferLayout;
                normalBufferLayout.SetBufferElements({ {ShaderDataType::VEC3, "normalPosition"} });

                normals = new VertexBufferObject(normalBufferLayout);
                normals->SetData(_normals.size() * sizeof(glm::vec3), _normals.data());
            }

            bool hasUVs = !_uv.empty();
            VertexBufferObject* uv = nullptr;
            if (hasUVs) {
                BufferLayout uvBufferLayout;
                uvBufferLayout.SetBufferElements({ {ShaderDataType::VEC2, "uvCoordinate"} });

                uv = new VertexBufferObject(uvBufferLayout);
                uv->SetData(_uv.size() * sizeof(glm::vec2), _uv.data());
            }

            ElementBufferObject* ebo = new ElementBufferObject();
            ebo->SetData(_triangles.size() * sizeof(unsigned), _triangles.data());

            _vao->ClearVBOs();
            _vao->AddVBO(vertices);

            if (hasNormals) {
                _vao->AddVBO(normals);
            }

            if (hasUVs) {
                _vao->AddVBO(uv);
            }
            _vao->SetEBO(ebo);

            _isDirty = false;
        }
    }

}
