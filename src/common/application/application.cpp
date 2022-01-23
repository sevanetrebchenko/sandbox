
#include "common/application/application.h"
#include "common/api/backend.h"
#include "common/application/time.h"
#include "common/ecs/ecs.h"

namespace Sandbox {

    Application::Application() {
    }

    Application::Application(int width, int height) {
        Window::Instance().SetDimensions(glm::ivec2(width, height));
    }

    Application::~Application() {
    }

    void Application::Init() {
        Window::Instance().Init();
        ECS::Instance().Init();
        sceneManager_.Init();
    }

    void Application::Run() {
        static float current = 0.0f;
        static float previous = 0.0f;
        static float dt = 0.0f;

        Window& window = Window::Instance();

        while (window.IsActive()) {
            // Prepare for a new frame.
            // dt calculations.
            current = static_cast<float>(glfwGetTime());
            dt = current - previous;
            previous = current;

            Time::Instance().dt = dt;

            // Clear canvas.
            Backend::Core::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            window.PollEvents(); // Needs to be called before ImGui::NewFrame().

            // ImGui.
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ECS& ecs = ECS::Instance();

            sceneManager_.Update();
            if (sceneManager_.SceneChangeRequested()) {
                ecs.Reset(); // Reset ECS data for new scene.
            }

            ecs.Update();

            // Scene processing.
            IScene* scene = sceneManager_.GetCurrentScene();
            if (scene) {
                // Events.
                if (window.CheckForResize()) {
                    scene->OnWindowResize(window.GetWidth(), window.GetHeight());
                }

                // Run currently active scene.
                scene->OnUpdate();

                scene->OnPreRender();
                scene->OnRender();
                scene->OnPostRender();

                scene->OnImGui();
            }


            // Scene rendering.
            ImGui::EndFrame();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            window.SwapBuffers();
        }
    }

    void Application::Shutdown() {
        sceneManager_.Shutdown();
        ECS::Instance().Shutdown();
        Window::Instance().Shutdown();
    }

    SceneManager& Application::GetSceneManager() {
        return sceneManager_;
    }

}
