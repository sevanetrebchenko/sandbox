
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

        if (_animator) {
            _animator->Update(dt);
        }

        if (pather_ && pather_->GetPath().IsValid()) {
            pather_->Update(dt);

            // Apply pather values to transform.
            _transform.SetPosition(pather_->GetCurrentPosition());

            glm::vec3 lookAt = glm::normalize(pather_->GetCurrentOrientation());
            glm::vec3 x = { 0.0f, 0.0f, 1.0f };
            glm::vec3 rotation = _transform.GetRotation();

            _transform.SetRotation(glm::vec3(rotation.x, rotation.y, glm::degrees(glm::acos(glm::dot(x, lookAt)))));
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


