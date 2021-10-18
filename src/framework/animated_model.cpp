
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
//        const std::vector<VQS>& finalTransformations = _animator->GetFinalBoneTransformations();
//        UniformBufferObject* ubo = UBOManager::GetInstance().GetUBO(2);
//        const UniformBlockLayout& uboLayout = ubo->GetUniformBlock().GetUniformBlockLayout();
//
//        ubo->Bind();
//
//        unsigned counter = 0;
//        const std::vector<UniformBufferElement>& elements = uboLayout.GetBufferElements();
//
//        // Set 'numBones' uniform.
//        int numBones = static_cast<int>(finalTransformations.size());
//        ubo->SetSubData(elements[counter].GetBufferOffset(), UniformBufferElement::UBOShaderDataTypeSize(elements[counter].GetShaderDataType()), static_cast<const void*>(&numBones));
//        ++counter;
//
//        // Set final bone transformation VQS structures.
//        for (std::size_t i = 0; i < numBones; ++i) {
//            const VQS& vqs = finalTransformations[i];
//
//            // Set 'translation' uniform.
//            glm::vec3 translation = vqs.GetTranslation();
//            ubo->SetSubData(elements[counter].GetBufferOffset(), UniformBufferElement::UBOShaderDataTypeSize(elements[counter].GetShaderDataType()), static_cast<const void*>(&translation));
//            ++counter;
//
//            // Set 'rotation' uniform.
//            glm::vec4 quaternion = vqs.GetOrientation().ToVec4();
//            ubo->SetSubData(elements[counter].GetBufferOffset(), UniformBufferElement::UBOShaderDataTypeSize(elements[counter].GetShaderDataType()), static_cast<const void*>(&quaternion));
//            ++counter;
//
//            // Set 'scale' uniform.
//            float scalingFactor = vqs.GetScalingFactor();
//            ubo->SetSubData(elements[counter].GetBufferOffset(), UniformBufferElement::UBOShaderDataTypeSize(elements[counter].GetShaderDataType()), static_cast<const void*>(&scalingFactor));
//            ++counter;
//        }
//
//        ubo->Unbind();
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


