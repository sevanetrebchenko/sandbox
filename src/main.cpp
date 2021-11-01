
// Scenes.
#include <scenes/project1/project1.h>
#include <scenes/project2/project2.h>
#include <scenes/deferred_rendering/deferred_rendering.h>

int main() {
    Sandbox::Scene *scene = new Sandbox::SceneProject2(1280, 720);
    try {
        scene->Init();
    }
    catch (std::runtime_error &exception) {
        std::cerr << exception.what() << std::endl;
        return 1;
    }

    scene->Run();
    scene->Shutdown();

    return 0;
}

#include <glm/glm.hpp>

