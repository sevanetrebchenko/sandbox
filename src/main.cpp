
#include <framework/application.h>

// Scenes
#include <scenes/deferred_rendering/deferred_rendering.h>

int main() {
    Sandbox::Application application;

    Sandbox::SceneManager& sceneManager = application.GetSceneManager();
    sceneManager.AddScene("Deferred Rendering", new Sandbox::SceneDeferredRendering(Sandbox::SceneOptions()));

    sceneManager.SetStartupScene("Deferred Rendering");

    application.Init();
    application.Run();
    application.Shutdown();

    return 0;
}
