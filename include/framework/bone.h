
#ifndef SANDBOX_BONE_H
#define SANDBOX_BONE_H

#include <sandbox_pch.h>

namespace Sandbox {

    struct BoneInfo {
        int boneID;
        glm::mat4 modelToBone; // Transforms vertex from model space to bone space.
    };

    // Stored animation key types.
    // dt stores the time to which the animation keys need to be interpolated to.
    struct KeyPosition {
        glm::vec3 position;
        float dt;
    };

    struct KeyRotation {
        glm::quat orientation; // TODO: replace with VQS.
        float dt;
    };

    struct KeyScale {
        glm::vec3 scale;
        float dt;
    };

    class Bone {
        public:
            Bone(int boneID, std::string  boneName, const aiNodeAnim* track);
            ~Bone();

            // Interpolate bone position, scale, and rotation based on the given animation time.
            // After update, GetLocalTransform() represents the model to bone space transform of this bone.
            void Update(float dt);

            [[nodiscard]] const glm::mat4& GetLocalTransform() const;
            [[nodiscard]] const std::string& GetName() const;
            [[nodiscard]] int GetBoneID() const;

        private:
            // Interpolate position, scale, and rotation.
            glm::mat4 Interpolate(float dt);

            // Assumes [start, end] : [0, duration]
            float GetFactor(float begin, float end, float duration);

            float Lerp(float start, float end, float duration);
            float Slerp(float start, float end, float duration);

            std::string _boneName;
            int _boneID;
            glm::mat4 _localTransform; // Local transformation for *this* bone only.

            std::vector<KeyPosition> _positions;
            std::vector<KeyScale> _scales;
            std::vector<KeyRotation> _rotations;
    };

}

#endif //SANDBOX_BONE_H
