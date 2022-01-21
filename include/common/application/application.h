
#ifndef SANDBOX_APPLICATION_H
#define SANDBOX_APPLICATION_H

#include "common/api/window.h"
#include "common/application/scene_manager.h"

namespace Sandbox {

    class Application {
        public:
            Application();
            Application(int width, int height);
            ~Application();

            void Init();
            void Run();
            void Shutdown();

            [[nodiscard]] SceneManager& GetSceneManager();

        private:
            SceneManager sceneManager_;
    };

}

#endif //SANDBOX_APPLICATION_H
