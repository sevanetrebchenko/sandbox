
#include <framework/animated_model.h>

#include <utility>
#include <framework/ubo_manager.h>

namespace Sandbox {

    AnimatedModel::AnimatedModel(std::string name) : Model(std::move(name)),
                                                     _animator(nullptr)
                                                     {
    }

    AnimatedModel::~AnimatedModel() {
        delete _animator;
    }

    void AnimatedModel::Update(float dt) {
        Model::Update(dt);
        _animator->Update(dt);

        // Update bone transformations in UBO.
        const std::vector<glm::mat4>& finalTransformations = _animator->GetFinalBoneTransformations();
        UniformBufferObject* ubo = UBOManager::GetInstance().GetUBO(2);
        const UniformBlockLayout& uboLayout = ubo->GetUniformBlock().GetUniformBlockLayout();

        ubo->Bind();

        unsigned counter = 0;
        const std::vector<UniformBufferElement>& elements = uboLayout.GetBufferElements();

        // Set 'numBones' uniform.
        std::size_t numBones = finalTransformations.size();
        ubo->SetSubData(elements[counter].GetBufferOffset(), UniformBufferElement::UBOShaderDataTypeSize(elements[counter].GetShaderDataType()), static_cast<const void*>(&numBones));

        ++counter;

        // Set final bone transformation matrices.
        for (std::size_t i = 0; i < numBones; ++i) {
            ubo->SetSubData(elements[counter].GetBufferOffset(), UniformBufferElement::UBOShaderDataTypeSize(elements[counter].GetShaderDataType()), static_cast<const void*>(&finalTransformations[i]));
            ++counter;
        }

        ubo->Unbind();
    }

    Animator *Sandbox::AnimatedModel::GetAnimator() const {
        return _animator;
    }

    Skeleton *AnimatedModel::GetSkeleton() const {
        return _skeleton;
    }

    void AnimatedModel::SetAnimator(Sandbox::Animator *animator) {
        _animator = animator;
    }

    void AnimatedModel::SetSkeleton(Skeleton *skeleton) {
        _skeleton = skeleton;
    }

}


