
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
        importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);

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
        std::vector<glm::vec4> boneIDs;
        std::vector<glm::vec4> boneWeights;

        // Default bone IDs / weights.
        for (unsigned i = 0; i < numVertices; ++i) {
            glm::vec4 ID;
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


        unsigned offset = skinnedMesh->GetVertices().size();  // Additional meshes get loaded at an offset into the same mesh.

        // Append vertices.
        {
            std::vector<glm::vec3> current = skinnedMesh->GetVertices();
            for (const glm::vec3& vertex : vertices) {
                current.emplace_back(vertex);
            }
            skinnedMesh->SetVertices(current);
        }

        // Append indices.
        {
            std::vector<unsigned> current = skinnedMesh->GetIndices();
            for (unsigned index : indices) {
                current.emplace_back(index + offset);
            }
            skinnedMesh->SetIndices(current);
        }

        // Append normals.
        {
            std::vector<glm::vec3> current = skinnedMesh->GetNormals();
            for (const glm::vec3& normal : normals) {
                current.emplace_back(normal);
            }
            skinnedMesh->SetNormals(current);
        }

        // Append UV.
        {
            std::vector<glm::vec2> current = skinnedMesh->GetUV();
            for (const glm::vec2& coordinate : uv) {
                current.emplace_back(coordinate);
            }
            skinnedMesh->SetUV(current);
        }

        // Append bone IDs.
        {
            std::vector<glm::vec4> current = skinnedMesh->GetBoneIDs();
            for (glm::vec4 boneID : boneIDs) {

                for (int i = 0; i < 4; ++i) {
                    if (boneID[i] > -0.5f) { // 0 is a valid bone index.
                        boneID[i] += offset; // Offset into vertex array.
                    }
                }

                current.emplace_back(boneID);
            }
            skinnedMesh->SetBoneIDs(current);
        }

        // Append bone weights.
        {
            std::vector<glm::vec4> current = skinnedMesh->GetBoneWeights();
            for (const glm::vec4& boneWeight : boneWeights) {
                current.emplace_back(boneWeight);
            }
            skinnedMesh->SetBoneWeights(current);
        }

        // Update any new bones.
        {
            std::unordered_map<std::string, BoneInfo> boneInfo = skinnedMesh->GetUniqueBoneMapping();
            unsigned numBones = skinnedMesh->GetBoneCount();

            bool addedNew = false;

            for (const std::pair<const std::string, BoneInfo>& data : uniqueBones) {
                // Found new bone.
                if (boneInfo.find(data.first) == boneInfo.end()) {
                    boneInfo[data.first] = data.second;
                    boneInfo[data.first].boneID += offset;
                    ++numBones;

                    addedNew = true;
                }
            }

            if (addedNew) {
                skinnedMesh->SetUniqueBoneMapping(boneInfo);
                skinnedMesh->SetBoneCount(numBones);
            }
        }
    }

}