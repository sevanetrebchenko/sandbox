
#include <common/application/application.h>

// Scenes
#include <scenes/deferred_rendering/deferred_rendering.h>
#include <scenes/cs562/project1/project1.h>
#include <scenes/cs562/project2/project2.h>
#include <scenes/cs562/project3/project3.h>
#include <scenes/cs562/project5/project5.h>

#include "common/api/shader/shader_preprocessor.h"
#include "common/api/shader/shader_uniform_lut.h"

using namespace Sandbox;

int main() {
    Application& application = Application::Instance();
    application.Init(1920, 1080);

    SceneManager& sceneManager = application.GetSceneManager();
    sceneManager.AddScene<SceneCS562Project5>("CS562: Project 5");
    sceneManager.SetActiveScene("CS562: Project 5");

    application.Run();
    application.Shutdown();

    return 0;
}
