
#ifndef SANDBOX_ANIMATED_MODEL_H
#define SANDBOX_ANIMATED_MODEL_H

#include <sandbox_pch.h>
#include <framework/model.h>
#include <framework/skeleton.h>
#include <framework/animator.h>
#include <framework/pather.h>
#include <framework/ik.h>

namespace Sandbox {

    class AnimatedModel : public Model {
        public:
            AnimatedModel(std::string name);
            ~AnimatedModel();

            void Update(float dt) override;

            [[nodiscard]] Animator* GetAnimator() const;
            [[nodiscard]] Skeleton* GetSkeleton() const;
            [[nodiscard]] Pather* GetPather() const;

            void SetAnimator(Animator* animator);
            void SetSkeleton(Skeleton* skeleton);
            void SetPather(Pather* pather);

        private:
            Animator* _animator;
            Skeleton* _skeleton;
            Pather* pather_;

            IKSolver ikSolver_;
    };

}

#endif //SANDBOX_ANIMATED_MODEL_H
