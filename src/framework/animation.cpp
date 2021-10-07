
#include <framework/animation.h>
#include <framework/imgui_log.h>
#include <framework/assimp_helper.h>
#include <framework/skinned_mesh.h>

namespace Sandbox {

    Animation::Animation(const std::string &filepath, Model *model) : _model(model) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate);

        if (!scene || !scene->mRootNode) {
            ImGuiLog& log = ImGuiLog::GetInstance();
            log.LogError("Failed to successfully import animation: '%s', error: %s", filepath.c_str(), importer.GetErrorString());
            throw std::runtime_error("Failed to import animation.");
        }

        aiAnimation* animation = scene->mAnimations[0];

        _duration = static_cast<float>(animation->mDuration);
        _speed = static_cast<float>(animation->mTicksPerSecond);

        // If loaded model contains missing bones, bones are found in the animation.
        // Continue loading missing bones into the model's mesh until fully complete.
        unsigned size = animation->mNumChannels;
        SkinnedMesh* mesh = dynamic_cast<SkinnedMesh*>(const_cast<Mesh*>(model->GetMesh()));
        assert(mesh); // This should not be called on a regular mesh.

        std::unordered_map<std::string, BoneInfo> uniqueBones = mesh->GetUniqueBoneMapping();
        unsigned numBones = mesh->GetBoneCount();

        bool added = false;

        for (int i = 0; i < size; i++) {
            aiNodeAnim* track = animation->mChannels[i];
            std::string boneName = track->mNodeName.C_Str();

            if (uniqueBones.find(boneName) == uniqueBones.end()) {
                added = true;
                uniqueBones[boneName].boneID = numBones++;
            }
        }

        // Update mesh bones.
        if (added) {
            mesh->SetUniqueBoneMapping(uniqueBones);
        }

        // Parse animation bone data.
        ProcessAnimationNode(scene->mRootNode, _root);
    }

    Animation::~Animation() {
    }

    Bone *Animation::GetBone(const std::string &boneName) {
        for (Bone& bone : _bones) {
            if (bone.GetName() == boneName) {
                return &bone;
            }
        }

        return nullptr;
    }

    float Animation::GetSpeed() const {
        return _speed;
    }

    float Animation::GetDuration() const {
        return _duration;
    }

    const AnimationNode &Animation::GetRootNode() const {
        return _root;
    }

    const Model *Animation::GetBoundModel() const {
        return _model;
    }

    void Animation::ProcessAnimationNode(const aiNode *root, AnimationNode &data) {
        assert(root);

        data._name = root->mName.C_Str();
        data._transform = GetGLMMatrix(root->mTransformation);

        for (int i = 0; i < root->mNumChildren; ++i) {
            // Process and push back children.
            AnimationNode node { };
            ProcessAnimationNode(root->mChildren[i], node);

            data._children.push_back(node);
        }
    }

}