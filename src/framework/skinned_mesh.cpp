
#include <framework/skinned_mesh.h>

namespace Sandbox {

    SkinnedMesh::SkinnedMesh(GLuint renderingPrimitive) : Mesh(renderingPrimitive) {
    }

    SkinnedMesh::~SkinnedMesh() {
    }

    SkinnedMesh::SkinnedMesh(const SkinnedMesh &other) : Mesh(other),
                                                         _boneIDs(other._boneIDs),
                                                         _boneWeights(other._boneWeights)
                                                         {
    }

    void SkinnedMesh::InitializeBuffers() {
        if (_isDirty) {
            if (!_vao) {
                _vao = new VertexArrayObject();
            }

            // Vertices.
            BufferLayout vertexBufferLayout;
            vertexBufferLayout.SetBufferElements({ {ShaderDataType::VEC3, "vertexPosition"} });

            VertexBufferObject* vertices = new VertexBufferObject(vertexBufferLayout);
            vertices->SetData(_vertices.size() * sizeof(glm::vec3), _vertices.data());


            // Normals.
            BufferLayout normalBufferLayout;
            normalBufferLayout.SetBufferElements({ {ShaderDataType::VEC3, "normalPosition"} });

            VertexBufferObject* normals = new VertexBufferObject(normalBufferLayout);
            normals->SetData(_normals.size() * sizeof(glm::vec3), _normals.data());


            // UV coordinates.
            BufferLayout uvBufferLayout;
            uvBufferLayout.SetBufferElements({ {ShaderDataType::VEC2, "uvCoordinate"} });

            VertexBufferObject* uv = new VertexBufferObject(uvBufferLayout);
            uv->SetData(_uv.size() * sizeof(glm::vec2), _uv.data());


            // Bone IDs.
            BufferLayout boneIDBufferLayout;
            boneIDBufferLayout.SetBufferElements({ {ShaderDataType::IVEC4, "boneIDs"} });

            VertexBufferObject* boneIDs = new VertexBufferObject(boneIDBufferLayout);
            boneIDs->SetData(_boneIDs.size() * sizeof(int), _boneIDs.data());


            // Bone weights.
            BufferLayout boneWeightBufferLayout;
            boneWeightBufferLayout.SetBufferElements({ {ShaderDataType::VEC4, "boneWeights"} });

            VertexBufferObject* boneWeights = new VertexBufferObject(boneWeightBufferLayout);
            boneWeights->SetData(_boneWeights.size() * sizeof(float), _boneWeights.data());


            ElementBufferObject* ebo = new ElementBufferObject();
            ebo->SetData(_triangles.size() * sizeof(unsigned), _triangles.data());

            _vao->ClearVBOs();

            _vao->AddVBO(vertices);
            _vao->AddVBO(normals);
            _vao->AddVBO(uv);
            _vao->AddVBO(boneIDs);
            _vao->AddVBO(boneWeights);

            _vao->SetEBO(ebo);

            _isDirty = false;
        }
    }

    void SkinnedMesh::SetBoneIDs(const std::vector<glm::ivec4> &boneIDs) {
        _boneIDs = boneIDs;
    }

    void SkinnedMesh::SetBoneWeights(const std::vector<glm::vec4> &boneWeights) {
        _boneWeights = boneWeights;
    }

    const std::vector<glm::ivec4> &SkinnedMesh::GetBoneIDs() const {
        return _boneIDs;
    }

    const std::vector<glm::vec4> &SkinnedMesh::GetBoneWeights() const {
        return _boneWeights;
    }

}


