
#ifndef SANDBOX_SCENE_MANAGER_H
#define SANDBOX_SCENE_MANAGER_H

#include <sandbox.h>
#include <framework/scene.h>
#include <framework/scene_data.h>

namespace Sandbox {

    class SceneManager {
        public:
            SceneManager();
            ~SceneManager();

            void OnImGui();

            template <typename SceneType, typename ...Args>
            void RegisterScene(const std::string& sceneName, const std::string& scenePath, Args&& ...args);

            void SetStartupScene(const std::string& sceneName);

            [[nodiscard]] IScene* GetCurrentScene();
            void SwitchScene(const std::string& sceneName);

        private:
            // Loads required scene data into SceneData structs. Establishes log + ImGui setup files.
            void LoadSceneData();

            std::list<SceneData> scenes_;
            std::list<SceneData>::iterator current_;
            bool initialized_;
            bool sceneLoaded_;
    };

}

#include <framework/scene_manager.tpp>

#endif //SANDBOX_SCENE_MANAGER_H
