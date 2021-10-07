
#include <framework/assimp_loader.h>
#include <framework/imgui_log.h>

namespace Sandbox {

    struct BoneInfo {
        int boneID;
        glm::mat4 modelToBone; // Transforms vertex from model space to bone space.
    };

    AssimpLoader &AssimpLoader::GetInstance() {
        static AssimpLoader loader;
        return loader;
    }

    AssimpLoader::AssimpLoader() {
    }

    AssimpLoader::~AssimpLoader() {
    }

    SkinnedMesh AssimpLoader::LoadFromFile(const std::string &filepath) {
        if (_loadedMeshes.find(filepath) != _loadedMeshes.end()) {
            return _loadedMeshes[filepath]; // Make copy.
        }

        SkinnedMesh mesh(GL_TRIANGLES);

        // Read via Assimp.
        Assimp::Importer importer;
        const aiScene* modelScene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

        if (!modelScene || modelScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !modelScene->mRootNode) {
            ImGuiLog& log = ImGuiLog::GetInstance();
            log.LogError("Assimp failed to open model scene '%s', error message: %s", filepath.c_str(), importer.GetErrorString());

            return mesh;
        }

        std::queue<aiNode*> sceneNodes;

        while (!sceneNodes.empty()) {
            aiNode* node = sceneNodes.front();
            sceneNodes.pop();

            for (unsigned i = 0; i < node->mNumMeshes; ++i) {
                // Load individual mesh.
                aiMesh* currentMesh = modelScene->mMeshes[node->mMeshes[i]];

                ProcessMesh(currentMesh, mesh);
            }

            // Add any children nodes.
            for (unsigned i = 0; i < node->mNumChildren; ++i) {
                sceneNodes.push(node->mChildren[i]);
            }
        }

        return mesh;
    }

    void AssimpLoader::ProcessMesh(aiMesh *mesh, SkinnedMesh& skinnedMesh) {
        // TODO: Bounding box
        // TODO: Skinning
        // TODO: Vertex / bone indexing
        // TODO: Skeleton class + BFS traveral for scene view.

        unsigned numVertices = mesh->mNumVertices;

        std::vector<glm::vec3> vertices (numVertices);
        std::vector<glm::vec3> normals (numVertices);
        std::vector<glm::vec2> uv (numVertices);

        // Vertex data.
        for (unsigned i = 0; i < numVertices; ++i) {
            vertices[i] = GetGLMVector(mesh->mVertices[i]);
            normals[i] = GetGLMVector(mesh->mNormals[i]);
            uv[i] = glm::vec2(0.0f, 0.0f); // No texture loading.
        }

        // Index data.
        std::vector<unsigned> indices (numVertices);
        unsigned counter = 0;

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices[counter++] = face.mIndices[j];
            }
        }

        // Bone data.
        std::vector<glm::ivec4> boneIDs (numVertices);
        std::vector<glm::vec4> boneWeights (numVertices);

        // Default bone IDs / weights.
        for (unsigned i = 0; i < numVertices; ++i) {
            for (int j = 0; j < 4; ++j) {
                boneIDs[i][j] = -1;
                boneWeights[i][j] = 0.0f;
            }
        }

        std::unordered_map<std::string, BoneInfo> uniqueBones;
        int totalNumBones = 0; // Running total of unique bones.

        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
            int boneID = -1;
            aiBone* bone = mesh->mBones[boneIndex];
            std::string boneName = bone->mName.C_Str();

            if (uniqueBones.find(boneName) == uniqueBones.end()) {
                // Bone with this name has not been encountered, register new entry.
                BoneInfo newBone { };

                newBone.boneID = totalNumBones;
                newBone.modelToBone = GetGLMMatrix(bone->mOffsetMatrix);

                // Register unique bone.
                uniqueBones[boneName] = newBone;
                boneID = totalNumBones;

                ++totalNumBones;
            }
            else {
                // Found duplicate bone.
                boneID = uniqueBones[boneName].boneID;
            }

            assert(boneID != -1); // Should never happen.

            aiVertexWeight* weights = bone->mWeights;
            unsigned numWeights = bone->mNumWeights;

            for (int i = 0; i < numWeights; ++i) {
                unsigned vertexID = weights[i].mVertexId;
                float weight = weights[i].mWeight;

                assert(vertexID < vertices.size());

                boneIDs[vertexID][i] = boneID;
                boneWeights[vertexID][i] = weight;
            }
        }

        // Start setting mesh.
        skinnedMesh.SetVertices(vertices);
        skinnedMesh.SetNormals(normals);
        skinnedMesh.SetIndices(indices);
        skinnedMesh.SetUV(uv);
        skinnedMesh.SetBoneIDs(boneIDs);
        skinnedMesh.SetBoneWeights(boneWeights);
    }

    glm::vec3 AssimpLoader::GetGLMVector(const aiVector3D &input) const {
        return { input.x, input.y, input.z };
    }

    glm::mat4 AssimpLoader::GetGLMMatrix(const aiMatrix4x4 &input) const {
        glm::mat4 matrix;

        matrix[0][0] = input.a1; matrix[1][0] = input.a2; matrix[2][0] = input.a3; matrix[3][0] = input.a4;
        matrix[0][1] = input.b1; matrix[1][1] = input.b2; matrix[2][1] = input.b3; matrix[3][1] = input.b4;
        matrix[0][2] = input.c1; matrix[1][2] = input.c2; matrix[2][2] = input.c3; matrix[3][2] = input.c4;
        matrix[0][3] = input.d1; matrix[1][3] = input.d2; matrix[2][3] = input.d3; matrix[3][3] = input.d4;

        return matrix;
    }

}