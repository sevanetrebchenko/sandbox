
#include <framework/ubo_manager.h>

#define MAX_NUM_LIGHTS 256
#define MAX_NUM_BONES 256

namespace Sandbox {

    UBOManager &UBOManager::GetInstance() {
        static UBOManager instance;
        return instance;
    }

    void UBOManager::AddUBO(UniformBlock &uniformBlock) {
        if (GetUBO(uniformBlock.GetBindingPoint())) {
            throw std::runtime_error("Overriding uniform block.");
        }

        UniformBufferObject* ubo = new UniformBufferObject();
        ubo->SetUniformBlock(uniformBlock);

        _ubos.push_back(ubo);
    }

    UniformBufferObject *UBOManager::GetUBO(unsigned bindingPoint) {
        for (UniformBufferObject* ubo : _ubos) {
            if (ubo->GetUniformBlock().GetBindingPoint() == bindingPoint) {
                return ubo;
            }
        }

        return nullptr;
    }

    UBOManager::UBOManager() {
        CreateMVPDataUBO();
        CreateLightingUBO();
        CreateAnimationDataUBO();
    }

    UBOManager::~UBOManager() {
    }

    void UBOManager::CreateMVPDataUBO() {
        // UBO for per-model model / camera transformations.
        std::vector<UniformBufferElement> elementList;
        // TODO
    }

    void UBOManager::CreateLightingUBO() {
        // UBO for lighting data.
        std::vector<UniformBufferElement> elementList;

        // Layout per light.
        for (int i = 0; i < MAX_NUM_LIGHTS; ++i) {
            elementList.emplace_back(ShaderDataType::BOOL, "isActive");
            elementList.emplace_back(ShaderDataType::VEC3, "position");
            elementList.emplace_back(ShaderDataType::VEC3, "ambientColor");
            elementList.emplace_back(ShaderDataType::VEC3, "diffuseColor");
            elementList.emplace_back(ShaderDataType::VEC3, "specularColor");
        }

        UniformBlockLayout lightingBlockLayout;
        lightingBlockLayout.SetBufferElements(0, 5, elementList);

        UniformBlock lightingBlock(1, lightingBlockLayout);
        AddUBO(lightingBlock);
    }

    void UBOManager::CreateAnimationDataUBO() {
        // UBO for per-model animation data.
        // Construct uniform block for animated models.
        std::vector<UniformBufferElement> elementList;

        // Layout per light.
        elementList.emplace_back(ShaderDataType::INT, "numBones");
        for (int i = 0; i < MAX_NUM_BONES; ++i) {
            elementList.emplace_back( ShaderDataType::MAT4, "finalBoneTransformations" );
        }

        UniformBlockLayout animationBlockLayout;
        animationBlockLayout.SetBufferElements(1, 1, elementList);

        UniformBlock animationBlock(2, animationBlockLayout);
        AddUBO(animationBlock);
    }
}
