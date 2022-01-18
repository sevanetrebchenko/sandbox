
#include <framework/application.h>
#include <framework/backend.h>

namespace Sandbox {

    Application::Application() {
    }

    Application::Application(int width, int height) {
        Window::Instance().SetDimensions(glm::ivec2(width, height));
    }

    Application::~Application() {
    }

    void Application::Init() {
        Window& window = Window::Instance();

        // Callback functions return cached state, do no polling themselves.
        // Keyboard input callback.
        glfwSetKeyCallback(window.GetNativeWindow(), [](GLFWwindow* window, int key, int scancode, int action, int mode) {
            IScene* activeScene = static_cast<IScene*>(glfwGetWindowUserPointer(window));
            if (activeScene) {
                activeScene->OnKeyboardInput(key, action);
            }
        });

        // Mouse input callback.
        glfwSetMouseButtonCallback(window.GetNativeWindow(), [](GLFWwindow* window, int button, int action, int mods) {
            IScene* activeScene = static_cast<IScene*>(glfwGetWindowUserPointer(window));
            if (activeScene) {
                activeScene->OnMouseInput(button, action);
            }
        });

        glfwSetScrollCallback(window.GetNativeWindow(), [](GLFWwindow* window, double xOffset, double yOffset) {
            IScene* activeScene = static_cast<IScene*>(glfwGetWindowUserPointer(window));
            if (activeScene) {
                // A normal mouse wheel, being vertical, provides offsets along the Y-axis.
                activeScene->OnMouseScroll(static_cast<float>(yOffset));
            }
        });
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

            // Clear canvas.
            Backend::Core::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            Backend::Core::ClearFlag(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // ImGui.
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            sceneManager_.Update();

            // Scene processing.
            IScene* scene = sceneManager_.GetCurrentScene();
            if (scene) {
                // Poll events processes the events that are waiting in the queue and calls input callbacks.
                // Ensure callback data is set for the event.
                glfwSetWindowUserPointer(window.GetNativeWindow(), static_cast<void*>(scene));
                window.PollEvents();

                // Additional events.
                if (window.CheckForResize()) {
                    scene->OnWindowResize(window.GetWidth(), window.GetHeight());
                }

                // Run currently active scene.
                scene->OnUpdate(dt);

                scene->OnPreRender();
                scene->OnRender();
                scene->OnPostRender();

                scene->OnImGui();
            }
            else {
                window.PollEvents();
            }


            // Scene rendering.
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            ImGui::EndFrame();

            window.SwapBuffers();
        }
    }

    void Application::Shutdown() {
        sceneManager_.Shutdown();
    }

    SceneManager& Application::GetSceneManager() {
        return sceneManager_;
    }

}
