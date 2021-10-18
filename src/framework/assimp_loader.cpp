
#include <framework/assimp_loader.h>
#include <framework/imgui_log.h>
#include <framework/assimp_helper.h>

namespace Sandbox {

    template <typename T>
    std::vector<T> Combine(const std::vector<T>& first, const std::vector<T>& second) {
        std::vector<T> combined;

        combined.reserve(first.size() + second.size());

        combined.insert(combined.end(), first.begin(), first.end());
        combined.insert(combined.end(), second.begin(), second.end());

        return combined;
    }

    AssimpLoader &AssimpLoader::GetInstance() {
        static AssimpLoader loader;
        return loader;
    }

    AssimpLoader::AssimpLoader() {
    }

    AssimpLoader::~AssimpLoader() {
    }

    AnimatedModel* AssimpLoader::LoadFromFile(const std::string &filepath) {
        // Read via Assimp.
        Assimp::Importer importer;
        importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);

        const aiScene* modelScene = importer.ReadFile(filepath, aiProcess_FlipUVs | aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes | aiProcessPreset_TargetRealtime_Fast | aiProcess_LimitBoneWeights | aiProcess_PopulateArmatureData);

        if (!modelScene || modelScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !modelScene->mRootNode) {
            ImGuiLog& log = ImGuiLog::GetInstance();
            log.LogError("Assimp failed to open model scene '%s', error message: %s", filepath.c_str(), importer.GetErrorString());
            throw std::runtime_error("Failed to load .glb model.");
        }

        SkinnedMesh* mesh = new SkinnedMesh(GL_TRIANGLES);
        Skeleton* skeleton = new Skeleton();

        // Parse scene root nodes.
        std::queue<aiNode*> sceneNodes;
        sceneNodes.push(modelScene->mRootNode);

        while (!sceneNodes.empty()) {
            aiNode* node = sceneNodes.front();
            sceneNodes.pop();

            for (unsigned i = 0; i < node->mNumMeshes; ++i) {
                // Load individual mesh.
                aiMesh* currentMesh = modelScene->mMeshes[node->mMeshes[i]];

                LoadMeshData(currentMesh, mesh);
                LoadSkeletonData(currentMesh, mesh, skeleton);
            }

            // Add any children nodes.
            for (unsigned i = 0; i < node->mNumChildren; ++i) {
                sceneNodes.push(node->mChildren[i]);
            }
        }

        mesh->Complete();

        // Load animations.
        Animator* animator = new Animator();
        animator->SetTarget(skeleton);
        LoadSceneAnimations(modelScene, skeleton, animator);

        AnimatedModel* model = new AnimatedModel(filepath);
        model->SetAnimator(animator);
        model->SetSkeleton(skeleton);
        model->SetMesh(mesh);

        return model;
    }

    void AssimpLoader::LoadMeshData(const aiMesh* mesh, SkinnedMesh* meshData) {
        std::size_t offset = meshData->GetVertices().size();

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uv;
        std::vector<unsigned> indices;

        // Vertex data.
        for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
            vertices.emplace_back(GetGLMVector(mesh->mVertices[i]));
            normals.emplace_back(GetGLMVector(mesh->mNormals[i]));
            uv.emplace_back(glm::vec2(0.0f, 0.0f)); // No textures.
        }

        // Index data.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.emplace_back(face.mIndices[j] + offset);
            }
        }

        // Combine with previous.
        meshData->SetVertices(Combine(meshData->GetVertices(), vertices));
        meshData->SetIndices(Combine(meshData->GetIndices(), indices));
        meshData->SetNormals(Combine(meshData->GetNormals(), normals));
        meshData->SetUV(Combine(meshData->GetUV(), uv));
    }

    void AssimpLoader::LoadSkeletonData(const aiMesh *mesh, SkinnedMesh *meshData, Skeleton *skeleton) {
        std::vector<glm::vec4> boneIDs;
        std::vector<glm::vec4> boneWeights;

        // Default bone IDs / weights.
        for (unsigned i = 0; i < meshData->GetVertices().size(); ++i) {
            glm::vec4 ID;
            glm::vec4 weight;

            for (int j = 0; j < 4; ++j) {
                ID[j] = -1;
                weight[j] = 0.0f;
            }

            boneIDs.emplace_back(ID);
            boneWeights.emplace_back(weight);
        }

        if (mesh->HasBones() && skeleton->_bones.empty()) {
            // Bone name -> (bone index, used in skeleton)
            std::unordered_map<std::string, std::pair<unsigned, bool>> boneMap;

            for (unsigned boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
                aiBone* bone = mesh->mBones[boneIndex];
                boneMap[bone->mName.C_Str()] = std::make_pair(boneIndex, false);
            }

            // Traverse bone hierarchy until bone has no parent (find root node).
            aiNode* rootBoneNode = mesh->mBones[0]->mNode;
            while (rootBoneNode) {
                if (boneMap.find(rootBoneNode->mName.C_Str()) == boneMap.end()) {
                    // Bone not present in the bone map.
                    break;
                }

                rootBoneNode = rootBoneNode->mParent;
            }

            if (rootBoneNode) {
                // Get all root bones.
                std::vector<std::pair<std::string, int>> rootBones;

                for (unsigned i = 0; i < rootBoneNode->mNumChildren; ++i) {
                    aiNode* childNode = rootBoneNode->mChildren[i];

                    if (auto iter = boneMap.find(childNode->mName.C_Str()); iter != boneMap.end()) {
                        // Found additional root node.
                        rootBones.emplace_back(iter->first, iter->second.first);
                    }
                }

                // Build skeleton + assign bone weights to vertices.
                for (const std::pair<std::string, int>& rootBoneData : rootBones) {
                    std::queue<aiNode*> armatures;
                    int numBones = static_cast<int>(skeleton->_bones.size()); // Used for determining bone IDs.

                    // Initialize with the root bone (total number of bones is the same as the final bone index).
                    skeleton->AddRoot(numBones);

                    Bone rootBone { };
                    rootBone._name = rootBoneData.first;
                    skeleton->AddBone(rootBone);

                    // Add root bone to be processed.
                    armatures.push(mesh->mBones[rootBoneData.second]->mNode);

                    // Create skeleton for this root node, assign vertex weights.
                    while (!armatures.empty()) {
                        aiNode* boneNode = armatures.front();
                        armatures.pop();

                        // Node processing.
                        std::pair<unsigned, bool>& boneInfo = boneMap[boneNode->mName.C_Str()];
                        aiBone* bone = mesh->mBones[boneInfo.first];
                        boneInfo.second = true; // Mark this bone as part of the skeleton.

                        // Initialize bone in skeleton.
                        Bone& skeletonBone = skeleton->_bones[numBones];
                        skeletonBone._index = numBones++;

                        // Get bone matrices.
                        skeletonBone._boneToModelMatrix = GetGLMMatrix(bone->mOffsetMatrix);
                        skeletonBone._boneToModelVQS = VQS(skeletonBone._boneToModelMatrix[3], Quaternion(skeletonBone._boneToModelMatrix));

                        skeletonBone._modelToBoneMatrix = glm::inverse(skeletonBone._boneToModelMatrix);
                        skeletonBone._modelToBoneVQS = VQS(skeletonBone._modelToBoneMatrix[3], Quaternion(skeletonBone._modelToBoneMatrix));

                        if (skeletonBone._parentIndex != -1) {
                            // Register bone as child bone of its parent.
                            skeleton->_bones[skeletonBone._parentIndex]._children.push_back(skeletonBone._index);
                        }

                        // Assign bone weight data.
                        for (unsigned i = 0; i < bone->mNumWeights; ++i) {
                            unsigned vertexID = bone->mWeights[i].mVertexId; // Vertex ID affected by this bone.
                            float weight = bone->mWeights[i].mWeight;

                            for (int j = 0; j < 4; ++j) {
                                // Find first bone that has not been initialized yet.
                                if (boneIDs[vertexID][j] == -1.0f) {
                                    boneIDs[vertexID][j] = static_cast<float>(skeletonBone._index);
                                    boneWeights[vertexID][j] = weight;
                                    break;
                                }
                            }
                        }

                        // Nodes that get pushed back are next in line for processing - BFS.
                        for (unsigned i = 0; i < boneNode->mNumChildren; ++i) {
                            aiNode* childNode = boneNode->mChildren[i];
                            auto iter = boneMap.find(childNode->mName.C_Str());

                            if (iter != boneMap.end()) {
                                // Bones that are already part of the skeleton should not be processed again.
                                if (!iter->second.second) {
                                    // Append new bone.
                                    Bone newSkeletonBone { };
                                    newSkeletonBone._name = iter->first;
                                    newSkeletonBone._parentIndex = numBones - 1;

                                    skeleton->AddBone(newSkeletonBone);

                                    // Process skeleton further.
                                    armatures.push(childNode);
                                }
                            }
                        }
                    }
                }
            }
        }

        meshData->SetBoneIDs(boneIDs);
        meshData->SetBoneWeights(boneWeights);
    }

    void AssimpLoader::LoadSceneAnimations(const aiScene *scene, Skeleton *skeleton, Animator *animator) {
        for (unsigned i = 0; i < scene->mNumAnimations; ++i) {
            aiAnimation* importedAnimation = scene->mAnimations[i];

            // Create animation.
            Animation* animation = new Animation();
            animation->_name = std::string(importedAnimation->mName.C_Str());
            animation->_duration = static_cast<float>(importedAnimation->mDuration);
            animation->_speed = static_cast<float>(importedAnimation->mTicksPerSecond);
            animation->_boneTracks.resize(skeleton->_bones.size());

            // Configure bone tracks.
            for (unsigned j = 0; j < importedAnimation->mNumChannels; ++j) {
                aiNodeAnim* animationChannel = importedAnimation->mChannels[j];
                std::string boneName = animationChannel->mNodeName.C_Str();

                auto iter = skeleton->_boneMapping.find(boneName);

                if (iter != skeleton->_boneMapping.end()) {
                    unsigned boneID = iter->second;

                    // Bone exists in the skeleton, register bone track.
                    Track& boneTrack = animation->_boneTracks[boneID];
                    boneTrack._name = boneName;

                    // Load position keys.
                    for (unsigned k = 0; k < animationChannel->mNumPositionKeys; ++k) {
                        aiVectorKey& animationPositionKey = animationChannel->mPositionKeys[k];

                        KeyPosition positionKey { };
                        positionKey.dt = static_cast<float>(animationPositionKey.mTime);
                        positionKey.position = GetGLMVector(animationPositionKey.mValue);

                        boneTrack._positionKeys.push_back(positionKey);
                    }

                    // Load rotation keys.
                    for (unsigned k = 0; k < animationChannel->mNumRotationKeys; ++k) {
                        aiQuatKey& animationRotationKey = animationChannel->mRotationKeys[k];

                        KeyRotation rotationKey { };
                        rotationKey.dt = static_cast<float>(animationRotationKey.mTime);
                        rotationKey.orientation = GetQuaternion(animationRotationKey.mValue); // Use custom quaternion.

                        boneTrack._rotationKeys.push_back(rotationKey);
                    }

                    // Load scale keys.
                    for (unsigned k = 0; k < animationChannel->mNumScalingKeys; ++k) {
                        aiVectorKey& animationScaleKey = animationChannel->mScalingKeys[k];

                        KeyScale scaleKey { };
                        scaleKey.dt = static_cast<float>(animationScaleKey.mTime);
                        scaleKey.scale = GetGLMVector(animationScaleKey.mValue);

                        boneTrack._scaleKeys.push_back(scaleKey);
                    }
                }
                else {
                    // Bone is not present in the animation.
                    // TODO: Add bone?
                }
            }

            // Add animation to animator.
            animator->AddAnimation(animation);
        }
    }

}