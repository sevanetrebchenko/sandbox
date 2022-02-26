
#ifndef SANDBOX_SCENE_H
#define SANDBOX_SCENE_H

#include "pch.h"

namespace Sandbox {

    class IScene {
        public:
            IScene();
            virtual ~IScene() = 0;

            virtual void OnInit();

            virtual void OnUpdate();

            virtual void OnPreRender();
            virtual void OnRender();
            virtual void OnPostRender();

            virtual void OnImGui();

            virtual void OnShutdown();

            // Callbacks.
            // Resize custom frame buffers + render attachments, configure cameras, etc.
            virtual void OnWindowResize(int width, int height);

            // 'Lock' changes to scene name / data directory.
            // Any attempts to change scene name / data directory AFTER the scene constructor are ignored.
            // Scene name + data configuration should be done in the scene constructor.
            void Lock();

            // Provide a public-facing name for the scene.
            // If not specified, data directory gets created from scene name.
            void SetName(const std::string& name);
            void SetDataDirectory(const std::string& dataDirectory);

            [[nodiscard]] const std::string& GetName() const;
            [[nodiscard]] const std::string& GetUILayoutFile() const;
            [[nodiscard]] const std::string& GetDataDirectory() const;
            [[nodiscard]] const std::string& GetShaderCacheDirectory() const;

        protected:
            std::string name_; // User-facing scene name.

            // Derived files / data directories.
            std::string dataDirectory_;
            std::string uiLayout_;
            std::string shaderCache_;

            bool isLocked_;
    };

}

#endif //SANDBOX_SCENE_H
