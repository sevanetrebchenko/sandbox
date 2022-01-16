
#ifndef SANDBOX_SCENE_H
#define SANDBOX_SCENE_H

#include <sandbox_pch.h>
#include <framework/camera.h>
#include <framework/window.h>
#include <framework/model_manager.h>
#include <framework/lighting_manager.h>

namespace Sandbox {

    class IScene {
        public:
            IScene(int width, int height);
            virtual ~IScene() = 0;

            virtual void OnInit();

            // Make sure to call base Scene::OnUpdate if this function is overridden.
            virtual void OnUpdate(float dt);

            virtual void OnPreRender();
            virtual void OnRender();
            virtual void OnPostRender();

            virtual void OnImGui();

            virtual void OnShutdown();

            virtual void OnWindowResize(int width, int height);

        protected:
            Camera _camera;

            ModelManager _modelManager;
            LightingManager _lightingManager;

        private:
            void LoadSceneData();

            void OnImGuiMenu();

            void BeginFrame();
            void Update(float dt);
            void EndFrame();

            void ProcessInput();
    };

}

#endif //SANDBOX_SCENE_H
