
#include <common/application/application.h>

// Scenes
#include <scenes/deferred_rendering/deferred_rendering.h>


int main() {

    Sandbox::Application application;
    application.Init();

    Sandbox::SceneManager& sceneManager = application.GetSceneManager();
    sceneManager.AddScene("Deferred Rendering", new Sandbox::SceneDeferredRendering());
    sceneManager.SetStartupScene("Deferred Rendering");

    application.Run();
    application.Shutdown();

    return 0;
}
