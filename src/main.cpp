
// Scenes.
#include <scenes/deferred_rendering/deferred_rendering.h>
#include <scenes/ascii_post_processing/ascii_post_processing.h>

int main() {

    Sandbox::Scene* scene = new Sandbox::SceneAsciiPostProcessing(1920, 1080);
    try {
        scene->Init();
    }
    catch (std::runtime_error& exception) {
        std::cerr << exception.what() << std::endl;
        return 1;
    }

    scene->Run();
    scene->Shutdown();

    return 0;
}
