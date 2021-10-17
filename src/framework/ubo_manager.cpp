
#include <framework/ubo_manager.h>

namespace Sandbox {

    UBOManager &UBOManager::GetInstance() {
        static UBOManager instance;
        return instance;
    }

    void UBOManager::AddUBO(UniformBlock &uniformBlock) {
        if (GetUBO(uniformBlock.GetBindingPoint())) {
            throw std::runtime_error("Overriding uniform block.");
        }

        UniformBufferObject& ubo = _ubos.emplace_back();
        ubo.SetUniformBlock(uniformBlock);
    }

    UniformBufferObject *UBOManager::GetUBO(unsigned bindingPoint) {
        for (UniformBufferObject& ubo : _ubos) {
            if (ubo.GetUniformBlock().GetBindingPoint() == bindingPoint) {
                return &ubo;
            }
        }

        return nullptr;
    }

    UBOManager::UBOManager() {
    }

    UBOManager::~UBOManager() {
    }
}
