
#ifndef SANDBOX_SCENE_MANAGER_H
#define SANDBOX_SCENE_MANAGER_H

#include <framework/scene.h>

namespace Sandbox {

    class SceneManager {
        public:
            SceneManager();
            ~SceneManager();

            void Init();
            void Update();
            void Shutdown();

            // Takes name by which the scene will be referenced in the editor.
            // Scenes will appear in the order they are added.
            void AddScene(const std::string& name, IScene* scene);

            [[nodiscard]] IScene* GetCurrentScene() const;

            void SetStartupScene(const std::string& name);

        private:
            struct SceneData {
            	SceneData(std::string sceneName, IScene* scene);
            	~SceneData();

                IScene* scene_;

                std::string prettyName_; // Public facing name, used for GUI and whatnot.
                std::string name_;
                std::string imGuiIniPath_;
            };

            [[nodiscard]] const SceneData* GetCurrentSceneData() const;

            void UnloadSceneData() const;
            void LoadSceneData() const;

            // Scene stubs, stored uninitialized.
            // Scenes are not going to be changed after initialization, we can get away with using a vector instead of a map.
            std::vector<SceneData> scenes_;
            int previousIndex_;
            int currentIndex_;
    };

}

#endif //SANDBOX_SCENE_MANAGER_H
