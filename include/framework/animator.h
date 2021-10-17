
#ifndef SANDBOX_ANIMATOR_H
#define SANDBOX_ANIMATOR_H

#include <sandbox_pch.h>
#include <framework/animation.h>
#include <framework/skeleton.h>

namespace Sandbox {

    class Animator {
        public:
            Animator();
            ~Animator();

            void Update(float dt);

            void SetTarget(Skeleton* skeleton);
            void AddAnimation(Animation* animation);

            void PlayAnimation(const std::string& animationName);
            void PlayAnimation(unsigned animationIndex);

            [[nodiscard]] const std::vector<glm::mat4>& GetFinalBoneTransformations() const;

            [[nodiscard]] const Animation* GetCurrentAnimation() const;

            [[nodiscard]] float GetPlaybackSpeed() const;
            void SetPlaybackSpeed(float playbackSpeed);

            [[nodiscard]] bool IsBindPoseActive() const;
            void SetBindPoseActive(bool active);

        private:
            void InterpolateBone(int boneIndex);

            Skeleton* _target;
            Animation* _selectedAnimation;
            std::vector<Animation*> _animations;

            std::vector<glm::mat4> _finalBoneTransformations;

            float _currentTime;
            float _playbackSpeed;

            bool _useBindPose;
            bool _reset; //
    };

}

#endif //SANDBOX_ANIMATOR_H
