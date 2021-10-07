
#include <framework/animator.h>
#include <framework/skinned_mesh.h>

namespace Sandbox {

    Animator::Animator() : _currentTime(0.0f),
                           _animation(nullptr)
                           {
        // TODO: support more than 100 bones.
        _finalBoneTransforms.reserve(100);
        for (int i = 0; i < 100; i++) {
            _finalBoneTransforms.emplace_back(1.0f);
        }
    }

    Animator::~Animator() {
    }

    void Animator::Update(float dt) {
        if (_animation) {
            _currentTime += std::fmod(_animation->GetSpeed() * dt, _animation->GetDuration()); // Needs to be a counter over the duraction of the animation.
            CalculateBoneTransform(_animation->GetRootNode(), glm::mat4(1.0f), dt);
        }
    }

    void Animator::PlayAnimation(Animation *animation) {
        _animation = animation;
        _currentTime = 0.0f; // Reset animation time for new animation.
    }

    const std::vector<glm::mat4> &Animator::GetFinalBoneTransforms() const {
        return _finalBoneTransforms;
    }

    void Animator::CalculateBoneTransform(const AnimationNode &node, glm::mat4 parentTransform, float dt) {
        Bone* bone = _animation->GetBone(node._name);
        glm::mat4 nodeTransform = node._transform;

        if (bone) {
            bone->Update(_currentTime);
            nodeTransform = bone->GetLocalTransform(); // Transform bone to world space.
        }

        glm::mat4 globalTransform = parentTransform * nodeTransform; // Transform bone to parent bone space.

        SkinnedMesh* mesh = dynamic_cast<SkinnedMesh*>(const_cast<Mesh*>(_animation->GetBoundModel()->GetMesh())); // TODO: remove const_cast.
        assert(mesh); // This should not be called on a regular mesh.

        const std::unordered_map<std::string, BoneInfo>& boneMapping = mesh->GetUniqueBoneMapping();
        if (auto iter = boneMapping.find(node._name); iter != boneMapping.end()) {
            // Node has offset from model position, apply it after parent transform.
            _finalBoneTransforms[iter->second.boneID] = globalTransform * iter->second.modelToBone;
        }

        // Process children transforms.
        for (const AnimationNode& child : node._children) {
            CalculateBoneTransform(child, globalTransform, dt);
        }
    }

}
