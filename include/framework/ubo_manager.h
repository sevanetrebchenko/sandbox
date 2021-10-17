
#ifndef SANDBOX_UBO_MANAGER_H
#define SANDBOX_UBO_MANAGER_H

#include <sandbox_pch.h>
#include <framework/buffer/ubo.h>

namespace Sandbox {

    class UBOManager {
        public:
            static UBOManager& GetInstance();

            void AddUBO(UniformBlock& uniformBlock);
            [[nodiscard]] UniformBufferObject* GetUBO(unsigned bindingPoint);

        private:
            UBOManager();
            ~UBOManager();

            void CreateMVPDataUBO();
            void CreateLightingUBO();
            void CreateAnimationDataUBO();

            std::vector<UniformBufferObject*> _ubos;
    };

}

#endif //SANDBOX_UBO_MANAGER_H
