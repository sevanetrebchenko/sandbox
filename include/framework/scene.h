
#ifndef SANDBOX_SCENE_H
#define SANDBOX_SCENE_H

#include <sandbox_pch.h>
#include <framework/camera.h>
#include <framework/model_manager.h>
#include <framework/lighting_manager.h>
#include <framework/shader_library.h>
#include <framework/material_library.h>
#include <framework/texture_library.h>

namespace Sandbox {

    struct SceneOptions {

        // Section: ImGui options.
        bool useDocking = true;
    };

    class IScene {
        public:
            IScene(SceneOptions options);
            virtual ~IScene() = 0;

            virtual void OnInit();

            // Call base Scene::OnUpdate if this function is overridden.
            virtual void OnUpdate(float dt);

            virtual void OnPreRender();
            virtual void OnRender();
            virtual void OnPostRender();

            virtual void OnImGui();

            virtual void OnShutdown();

            virtual void OnWindowResize(int width, int height);
            virtual void OnKeyboardInput(int key, int action);
            virtual void OnMouseInput(int button, int action);
            virtual void OnMouseScroll(float offset);

        protected:
            // Configure various per-scene properties.
            SceneOptions options_;

            ICamera* camera_;

            ModelManager modelManager_;
            LightingManager lightingManager_;

            ShaderLibrary shaderLibrary_;
            MaterialLibrary materialLibrary_;
            TextureLibrary textureLibrary_;
    };

}

#endif //SANDBOX_SCENE_H
