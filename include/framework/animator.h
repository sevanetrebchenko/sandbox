
#ifndef SANDBOX_ANIMATOR_H
#define SANDBOX_ANIMATOR_H

#include <sandbox_pch.h>
#include <framework/animation.h>
#include <framework/skeleton.h>
#include <framework/vqs.h>
#include <framework/interpolation.h>

namespace Sandbox {

    class Animator {
        public:
            Animator();
            ~Animator();

            // Returns if animation should be processed.
            [[nodiscard]] bool ProcessBindPose();
            void ComputeLocalBoneTransformations(float dt);
            void ComputeFinalBoneTransformations();
            void ShiftBones() {
                // Shift bone to proper position AFTER calculating ALL bone matrices.
                for (Bone& bone : _target->_bones) {
                    int boneIndex = bone._index;
                    boneVQSTransformations_[boneIndex] = boneVQSTransformations_[boneIndex] * bone._boneToModelVQS;
                }
            }

            void SetTarget(Skeleton* skeleton);
            void AddAnimation(Animation* animation);

            void PlayAnimation(const std::string& animationName);
            void PlayAnimation(unsigned animationIndex);

            [[nodiscard]] const std::vector<VQS>& GetBoneTransformations() const;

            [[nodiscard]] const Animation* GetCurrentAnimation() const;

            [[nodiscard]] float GetPlaybackSpeed() const;
            void SetPlaybackSpeed(float playbackSpeed);

            [[nodiscard]] bool IsBindPoseActive() const;
            void SetBindPoseActive(bool active);

            [[nodiscard]] KeyInterpolationMethod GetKeyInterpolationMethod() const;
            void SetKeyInterpolationMethod(KeyInterpolationMethod method);

            [[nodiscard]] QuaternionInterpolationMethod GetQuaternionInterpolationMethod() const;
            void SetQuaternionInterpolationMethod(QuaternionInterpolationMethod method);

            // IK.
            void IK(const glm::mat4& modelMatrix);

            [[nodiscard]] int GetEndEffectorBoneIndex() const;
            void SetEndEffectorBoneIndex(int index);

            [[nodiscard]] const glm::vec3& GetIKTargetPosition() const;
            void SetIKTargetPosition(const glm::vec3& targetPosition);

            void UseIK(bool useIK);
            [[nodiscard]] bool Initialized() const;

            float GetIKChainLength();
            glm::vec3 GetEndEffectorPosition(const glm::mat4& modelMatrix) const;

        private:
            void InterpolateBone(int boneIndex);
            void DefaultKeyInterpolation(int boneIndex);
            void IncrementalKeyInterpolation(int boneIndex);

            void SolveIKChainFABRIK(const glm::mat4& modelMatrix);
            void WorldToIKChain(); // Converts world position IK chain back into local space.
            void IterateBackwards(const glm::vec3& goalPosition);
            void IterateForwards(const glm::vec3& rootPosition);

            void SolveIKChainCCD(const glm::mat4& modelMatrix);

            void ComputeIKChain();

            void ApplyRotation(const Quaternion& rotation, int boneIndex);

            VQS GetGlobalTransformation(int index);

            Skeleton* _target;
            Animation* _selectedAnimation;
            std::vector<Animation*> _animations;

            KeyInterpolationMethod _keyInterpolationMethod;
            QuaternionInterpolationMethod _quaternionInterpolationMethod;

            std::vector<VQS> boneVQSTransformations_;
            std::vector<int> chain_;
            int endEffectorIndex_;

            std::vector<glm::vec3> joints_;
            std::vector<float> distances_;

            glm::vec3 targetPosition_;

            float _currentTime;
            float _playbackSpeed;

            bool init;

            bool _useBindPose;
            bool _reset;
    };

}

#endif //SANDBOX_ANIMATOR_H
