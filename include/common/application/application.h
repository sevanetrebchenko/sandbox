
#ifndef SANDBOX_APPLICATION_H
#define SANDBOX_APPLICATION_H

#include "common/api/window.h"
#include "common/application/scene_manager.h"
#include "common/utility/singleton.h"

namespace Sandbox {

    class Application : public Singleton<Application> {
        public:
            REGISTER_SINGLETON(Application);

            void Init(int width, int height);
            void Run();
            void Shutdown();

            [[nodiscard]] SceneManager& GetSceneManager();

        private:
            Application();
            ~Application() override;

            SceneManager sceneManager_;
    };

}

#endif //SANDBOX_APPLICATION_H
