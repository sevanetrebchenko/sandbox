
#include <common/application/application.h>

// Scenes
#include <scenes/deferred_rendering/deferred_rendering.h>
#include <scenes/cs562/project1/project1.h>

using namespace Sandbox;

int main() {
    Application& application = Application::Instance();
    application.Init(1920, 1080);

    SceneManager& sceneManager = application.GetSceneManager();
    sceneManager.AddScene<SceneCS562Project1>("CS562: Project 1");
    sceneManager.SetActiveScene("CS562: Project 1");

    application.Run();
    application.Shutdown();

    return 0;
}
