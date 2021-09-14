
#ifndef SANDBOX_LIGHTING_MANAGER_H
#define SANDBOX_LIGHTING_MANAGER_H

#include <sandbox.h>
#include <framework/buffer/ubo.h>
#include <framework/light.h>
#include <framework/singleton.h>

namespace Sandbox {

    class LightingManager : public Singleton<LightingManager> {
        public:
            REGISTER_SINGLETON(LightingManager);

            void Initialize() override;
            void Shutdown() override;

            void OnImGui();

            void Update();
            void AddLight(const Light& light);

            void Clear();

        private:
            LightingManager();
            ~LightingManager() override;

            void ConstructUniformBlock();

            unsigned _numActiveLights;
            bool _isDirty;

            UniformBufferObject _lightingUBO;
            std::list<Light> _lights;
    };

}

#endif //SANDBOX_LIGHTING_MANAGER_H
