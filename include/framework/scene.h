
#ifndef SANDBOX_SCENE_H
#define SANDBOX_SCENE_H

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
            // Resize custom framebuffers + render attachments, configure cameras, etc.
            virtual void OnWindowResize(int width, int height);
    };

}

#endif //SANDBOX_SCENE_H
