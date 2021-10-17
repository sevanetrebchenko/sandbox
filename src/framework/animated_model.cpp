
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

//        // Update bone transformations in UBO.
//        const std::vector<glm::mat4>& finalTransformations = _animator->GetFinalBoneTransformations();
//
//        UniformBufferObject* ubo = UBOManager::GetInstance().GetUBO(1);
//        const UniformBlockLayout& uboLayout = ubo->GetUniformBlock().GetUniformBlockLayout();
//
//        ubo->Bind();
//
//        // Set numBones uniform in UBO.
//        {
//            const UniformBufferElement& element = uboLayout.GetBufferElements()[0];
//            unsigned elementOffset = element.GetBufferOffset();
//            unsigned elementDataSize = UniformBufferElement::UBOShaderDataTypeSize(element.GetShaderDataType());
//            int numBones = finalTransformations.size();
//            ubo->SetSubData(elementOffset, elementDataSize, static_cast<const void*>(&numBones));
//        }
//
//        // Set final bone transformation matrices.
//        for (int i = 1; i < finalTransformations.size(); ++i) {
//            const UniformBufferElement& element = uboLayout.GetBufferElements()[i];
//            unsigned elementOffset = element.GetBufferOffset();
//            unsigned elementDataSize = UniformBufferElement::UBOShaderDataTypeSize(element.GetShaderDataType());
//
//            ubo->SetSubData(elementOffset, elementDataSize, static_cast<const void*>(finalTransformations.data()));
//        }
//
//        ubo->Unbind();
    }

    Animator *Sandbox::AnimatedModel::GetAnimator() const {
        return _animator;
    }

    void AnimatedModel::SetAnimator(Sandbox::Animator *animator) {
        _animator = animator;
    }

}


