
#ifndef SANDBOX_SCENE_H
#define SANDBOX_SCENE_H

#include <sandbox_pch.h>
#include <framework/camera.h>
#include <framework/window.h>
#include <framework/model_manager.h>

namespace Sandbox {

    class Scene {
        public:
            Scene(std::string name, int width, int height);
            virtual ~Scene();

            void Init();
            void Run();
            void Shutdown();

        protected:
            virtual void OnInit() = 0;

            virtual void OnUpdate(float dt) = 0;

            virtual void OnPreRender() = 0;
            virtual void OnRender() = 0;
            virtual void OnPostRender() = 0;
            virtual void OnImGui() = 0;

            virtual void OnShutdown() = 0;

            // Data provided to derived scenes by default.
            Window _window;
            Camera _camera;
            ModelManager _modelManager;
            std::string _dataDirectory;

        private:
            void LoadSceneData();

            void OnImGuiMenu();

            void BeginFrame();
            void EndFrame();

            void ProcessInput();

            std::string _sceneName;
            std::string _imGuiIniName;
            std::string _imGuiLogName;
            float _dt;
            float _currentFrameTime;
            float _previousFrameTime;
    };

}

#endif //SANDBOX_SCENE_H
