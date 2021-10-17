
#ifndef SANDBOX_ANIMATED_MODEL_H
#define SANDBOX_ANIMATED_MODEL_H

#include <sandbox_pch.h>
#include <framework/model.h>
#include <framework/skeleton.h>
#include <framework/animator.h>

namespace Sandbox {

    class AnimatedModel : public Model {
        public:
            AnimatedModel(std::string name);
            ~AnimatedModel();

            void Update(float dt) override;

            [[nodiscard]] Animator* GetAnimator() const;
            void SetAnimator(Animator* animator);

        private:
            Animator* _animator;
    };

}

#endif //SANDBOX_ANIMATED_MODEL_H
