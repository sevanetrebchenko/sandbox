
#include <common/application/application.h>

// Scenes
#include <scenes/deferred_rendering/deferred_rendering.h>
#include <scenes/cs562/project1/project1.h>


int main() {

    Sandbox::Application application;
    application.Init();

    Sandbox::SceneManager& sceneManager = application.GetSceneManager();
//    sceneManager.AddScene("Deferred Rendering", new Sandbox::SceneDeferredRendering());
    sceneManager.AddScene("CS562: Project 1", new Sandbox::SceneCS562Project1());

    sceneManager.SetStartupScene("CS562: Project 1");

    application.Run();
    application.Shutdown();

    return 0;
}
