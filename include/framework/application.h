
#ifndef SANDBOX_APPLICATION_H
#define SANDBOX_APPLICATION_H

#include <sandbox.h>

#include <framework/scene_manager.h>
#include <framework/window.h>
#include <framework/material_library.h>
#include <framework/shader_library.h>
#include <framework/model_manager.h>

namespace Sandbox {

    class Application {
        public:
            Application();
            ~Application();

            void Initialize();
            void Run();
            void Shutdown();

            [[nodiscard]] SceneManager* GetSceneManager();

        private:
            void BeginFrame();

            void Update();
            void OnImGui();

            void EndFrame();

            void ProcessInput();

            SceneManager sceneManager_;

            float dt_;
            float currentFrameTime_;
            float previousFrameTime_;
    };

}

#endif //SANDBOX_APPLICATION_H
