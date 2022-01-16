
#include <framework/application.h>

namespace Sandbox {


    Application::Application() {

    }

    Application::~Application() {

    }

    void Application::Init() {

    }

    void Application::Run() {
        sceneManager_.Update();

        if (window_.CheckForResize()) {
            sceneManager_.GetCurrentScene()->OnWindowResize(window_.GetWidth(), window_.GetHeight());
        }
    }

    void Application::Shutdown() {

    }
}
