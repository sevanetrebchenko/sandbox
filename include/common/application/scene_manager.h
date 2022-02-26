
#pragma once

#include "pch.h"
#include "common/application/scene.h"

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
            template <typename T>
            void AddScene(const std::string& sceneName);

            [[nodiscard]] IScene* GetActiveScene() const;
            [[nodiscard]] bool SceneChangeRequested() const;

            void SwitchScenes();

            void SetActiveScene(const std::string& name);

        private:
            // Interface for capturing scene type into a class to be able to destroy/create scenes on load.
            // Type erasure of scene type into void*, as SceneManager only needs to interface with IScene*.
            struct ISceneType {
                explicit ISceneType(const std::string& name);
                virtual ~ISceneType();

                virtual void Create() = 0;
                virtual void Destroy() = 0;

                IScene* scene_;
                std::string name_; // Name the scene was registered in the SceneManager with.
                                   // Scenes will default to this name if not otherwise set in scene either the constructor or OnInit function.
            };

            template <typename T>
            struct SceneType : public ISceneType {
                explicit SceneType(const std::string& name);
                ~SceneType() override;

                void Create() override;
                void Destroy() override;
            };

            [[nodiscard]] bool ValidateSceneName(const std::string& name) const;

            [[nodiscard]] ISceneType* GetActiveSceneType() const;
            void UnloadSceneData() const;
            void LoadSceneData() const;

            std::vector<ISceneType*> scenes_;
            int previousIndex_;
            int currentIndex_;

            bool sceneChangeRequested_;
    };

}

#include "common/application/scene_manager.tpp"
