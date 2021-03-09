
#ifndef SANDBOX_LIGHTING_MANAGER_H
#define SANDBOX_LIGHTING_MANAGER_H

#include <sandbox_pch.h>
#include <framework/buffer/ubo.h>
#include <framework/light.h>

namespace Sandbox {

    class LightingManager {
        public:
            LightingManager();
            ~LightingManager();

            void OnImGui();

            void Update();
            void AddLight(const Light& light);

        private:
            void ConstructUniformBlock();

            unsigned _numActiveLights;
            bool _isDirty;

            UniformBufferObject _lightingUBO;
            std::list<Light> _lights;
    };

}

#endif //SANDBOX_LIGHTING_MANAGER_H
