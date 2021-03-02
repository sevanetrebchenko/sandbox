
// Scenes.
#include <scenes/deferred_rendering.h>

int main() {

    Sandbox::Scene* scene = new Sandbox::SceneDeferredRendering(1920, 1080);
    try {
        scene->Init();
    }
    catch (std::runtime_error& exception) {
        std::cerr << exception.what() << std::endl;
    }

    scene->Run();
    scene->Shutdown();

    return 0;
}
