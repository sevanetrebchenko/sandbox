
#include <framework/animated_model.h>

#include <utility>
#include <framework/ubo_manager.h>

namespace Sandbox {

    AnimatedModel::AnimatedModel(std::string name) : Model(std::move(name)),
                                                     _animator(nullptr),
                                                     pather_(nullptr)
                                                     {
    }

    AnimatedModel::~AnimatedModel() {
        delete _animator;
        delete _skeleton;
        delete pather_;
    }

    void AnimatedModel::Update(float dt) {
        Model::Update(dt);

        if (_animator && _animator->ProcessBindPose()) {
            // Compute local (hierarchical) transformations for this model.
            _animator->ComputeLocalBoneTransformations(dt);

            // IK...
            if (_animator->Initialized()) {
                _animator->IK(_transform.GetMatrix());
            }

            // Blending...

            // Compute final transformations - ready to send to GPU.
            _animator->ComputeFinalBoneTransformations();
            _animator->ShiftBones();
        }

        if (pather_ && pather_->GetPath().IsValid()) {
            if (_animator->Initialized()) {
                pather_->Update(dt);
            }

            // Apply values to transform.
            _transform.SetPosition(pather_->GetCurrentPosition());
            _transform.SetRotationOffset(glm::quatLookAt(pather_->GetCurrentOrientation(), glm::vec3(0.0f, 1.0f, 0.0f)));
        }

    }

    Animator *Sandbox::AnimatedModel::GetAnimator() const {
        return _animator;
    }

    Skeleton *AnimatedModel::GetSkeleton() const {
        return _skeleton;
    }

    Pather *AnimatedModel::GetPather() const {
        return pather_;
    }

    void AnimatedModel::SetAnimator(Sandbox::Animator *animator) {
        _animator = animator;
    }

    void AnimatedModel::SetSkeleton(Skeleton *skeleton) {
        _skeleton = skeleton;
    }

    void AnimatedModel::SetPather(Pather *pather) {
        pather_ = pather;
    }

}


