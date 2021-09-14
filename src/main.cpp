
// Scenes.
#include <scenes/deferred_rendering/deferred_rendering.h>
#include <scenes/ascii_post_processing/ascii_post_processing.h>

#include <framework/application.h>

using namespace Sandbox;

int main() {
    Application application;

    // Register scenes.
    SceneManager* sceneManager = application.GetSceneManager();
    sceneManager->RegisterScene<SceneDeferredRendering>("Deferred Rendering", "data/scenes/deferred_rendering");
    sceneManager->RegisterScene<SceneAsciiPostProcessing>("ASCII Post Processing", "data/scenes/ascii_post_processing");

    // Startup scene.
    sceneManager->SetStartupScene("Deferred Rendering");

    try {
        application.Initialize();
        application.Run();
    }
    catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    application.Shutdown();

    return 0;
}
