
#ifndef SANDBOX_APPLICATION_H
#define SANDBOX_APPLICATION_H

#include <framework/scene_manager.h>

namespace Sandbox {

    class Application {
        public:
            Application();
            ~Application();

            void Init();
            void Run();
            void Shutdown();

        private:
            Window window_;
            SceneManager sceneManager_;
    };

}

#endif //SANDBOX_APPLICATION_H
