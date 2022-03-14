
#include <common/application/application.h>

// Scenes
#include <scenes/deferred_rendering/deferred_rendering.h>
#include <scenes/cs562/project1/project1.h>
#include <scenes/cs562/project2/project2.h>

#include "common/api/shader/shader_preprocessor.h"

using namespace Sandbox;

int main() {
    ShaderPreprocessor::Instance().AddIncludeDirectory("assets/shaders");
    ShaderPreprocessor::Instance().AddIncludeDirectory("assets/models");

    ShaderPreprocessor::Instance().ProcessSource("\n"
                                                 "#type adfasdfsaf\n"
                                                 "\n"
                                                 "          #               version      330    core      ddd d dd\n"
                                                 "\n"
                                                 "\n"
                                                 "\n"
                                                 "layout (location = 0) in vec3 vertexPosition;\n"
                                                 "layout (location = 1) in vec3 vertexNormal;\n"
                                                 "\n"
                                                 "uniform mat4 cameraTransform;\n"
                                                 "uniform mat4 modelTransform;\n"
                                                 "\n"
                                                 "void main() {\n"
                                                 "    gl_Position = cameraTransform * modelTransform * vec4(vertexPosition, 1.0f);\n"
                                                 "}");


    ShaderPreprocessor::Instance().ProcessFile("assets/shaders/test.glsl");
//    ShaderPreprocessor::Instance().ProcessFile("assets/shaders/model.vert");



    Application& application = Application::Instance();
    application.Init(1920, 1080);

    SceneManager& sceneManager = application.GetSceneManager();
//    sceneManager.AddScene<SceneCS562Project1>("CS562: Project 1");
    sceneManager.AddScene<SceneCS562Project2>("CS562: Project 2");
    sceneManager.SetActiveScene("CS562: Project 2");

    application.Run();
    application.Shutdown();

    return 0;
}
