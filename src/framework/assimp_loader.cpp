
#include <framework/assimp_loader.h>
#include <framework/imgui_log.h>
#include <framework/assimp_helper.h>

namespace Sandbox {



    AssimpLoader &AssimpLoader::GetInstance() {
        static AssimpLoader loader;
        return loader;
    }

    AssimpLoader::AssimpLoader() {
    }

    AssimpLoader::~AssimpLoader() {
    }

    SkinnedMesh* AssimpLoader::LoadFromFile(const std::string &filepath) {
        SkinnedMesh* mesh = new SkinnedMesh(GL_TRIANGLES);

        // Read via Assimp.
        Assimp::Importer importer;
        const aiScene* modelScene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

        if (!modelScene || modelScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !modelScene->mRootNode) {
            ImGuiLog& log = ImGuiLog::GetInstance();
            log.LogError("Assimp failed to open model scene '%s', error message: %s", filepath.c_str(), importer.GetErrorString());
            throw std::runtime_error("Failed to load .glb model.");
        }

        std::queue<aiNode*> sceneNodes;
        sceneNodes.push(modelScene->mRootNode);

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

    void AssimpLoader::ProcessMesh(aiMesh *mesh, SkinnedMesh* skinnedMesh) {
        // TODO: Bounding box
        // TODO: Skinning
        // TODO: Vertex / bone indexing
        // TODO: Skeleton class + BFS traveral for scene view.

        unsigned numVertices = mesh->mNumVertices;

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uv;

        // Vertex data.
        for (unsigned i = 0; i < numVertices; ++i) {
            vertices.emplace_back(GetGLMVector(mesh->mVertices[i]));
            normals.emplace_back(GetGLMVector(mesh->mNormals[i]));
            uv.emplace_back(glm::vec2(0.0f, 0.0f)); // No textures.
        }

        // Index data.
        std::vector<unsigned> indices;
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.emplace_back(face.mIndices[j]);
            }
        }

        // Bone data.
        std::vector<glm::ivec4> boneIDs;
        std::vector<glm::vec4> boneWeights;

        // Default bone IDs / weights.
        for (unsigned i = 0; i < numVertices; ++i) {
            glm::ivec4 ID;
            glm::vec4 weight;

            for (int j = 0; j < 4; ++j) {
                ID[j] = -1;
                weight[j] = 0.0f;
            }

            boneIDs.emplace_back(ID);
            boneWeights.emplace_back(weight);
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

                for (int j = 0; j < 4; ++j) {
                    // Find first bone that has not been initialized.
                    if (boneIDs[vertexID][j] < 0) {
                        boneWeights[vertexID][j] = weight;
                        boneIDs[vertexID][j] = boneID;
                        break;
                    }
                }
            }
        }

        // Start setting mesh.
        skinnedMesh->SetVertices(vertices);
        skinnedMesh->SetNormals(normals);
        skinnedMesh->SetIndices(indices);
        skinnedMesh->SetUV(uv);
        skinnedMesh->SetBoneIDs(boneIDs);
        skinnedMesh->SetBoneWeights(boneWeights);
        skinnedMesh->SetUniqueBoneMapping(uniqueBones);
        skinnedMesh->SetBoneCount(totalNumBones);
    }

}