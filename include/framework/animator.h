
#ifndef SANDBOX_ANIMATOR_H
#define SANDBOX_ANIMATOR_H

#include <sandbox_pch.h>
#include <framework/animation.h>

namespace Sandbox {

    class Animator {
        public:
            Animator();
            ~Animator();

            void Update(float dt);
            void PlayAnimation(Animation* animation);

            [[nodiscard]] const std::vector<glm::mat4>& GetFinalBoneTransforms() const;

        private:
            void CalculateBoneTransform(const AnimationNode& node, glm::mat4 parentTransform, float dt);

            std::vector<glm::mat4> _finalBoneTransforms;
            Animation* _animation;

            float _currentTime;
    };

}

#endif //SANDBOX_ANIMATOR_H
