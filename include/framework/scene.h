
#ifndef SANDBOX_SCENE_H
#define SANDBOX_SCENE_H

#include <sandbox.h>
#include <framework/camera.h>
#include <framework/window.h>
#include <framework/model_manager.h>
#include <framework/lighting_manager.h>
#include <framework/shader_library.h>

namespace Sandbox {

    class SceneData; // Forward declaration.

    class IScene {
        public:
            IScene();
            virtual ~IScene();

            virtual void OnInit() = 0;

            virtual void OnUpdate(float dt) = 0;

            virtual void OnPreRender() = 0;
            virtual void OnRender() = 0;
            virtual void OnPostRender() = 0;
            virtual void OnImGui() = 0;

            virtual void OnShutdown() = 0;

            // Data provided to derived scenes by default.
            Camera camera_;
            SceneData* sceneData_;
    };

}

#endif //SANDBOX_SCENE_H
