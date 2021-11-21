
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
            // Returns a chain starting from the end effector and going to the root.
            [[nodiscard]] const std::vector<VQS*>& GetIKChain() const;

            [[nodiscard]] int GetEndEffectorBoneIndex() const;
            void SetEndEffectorBoneIndex(int index);

            [[nodiscard]] const glm::vec3& GetIKTargetPosition() const;
            void SetIKTargetPosition(const glm::vec3& targetPosition);

        private:
            void InterpolateBone(int boneIndex);
            void DefaultKeyInterpolation(int boneIndex);
            void IncrementalKeyInterpolation(int boneIndex);

            void ComputeIKChain();

            Skeleton* _target;
            Animation* _selectedAnimation;
            std::vector<Animation*> _animations;

            KeyInterpolationMethod _keyInterpolationMethod;
            QuaternionInterpolationMethod _quaternionInterpolationMethod;

            std::vector<VQS> boneVQSTransformations_;
            std::vector<VQS*> chain_;
            int endEffectorIndex_;

            glm::vec3 targetPosition_;

            float _currentTime;
            float _playbackSpeed;

            bool _useBindPose;
            bool _reset;
    };

}

#endif //SANDBOX_ANIMATOR_H
