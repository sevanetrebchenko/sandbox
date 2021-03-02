
#include <framework/scene.h>

namespace Sandbox {

    Scene::Scene(const std::string& name, int width, int height) : _window( { name, width, height } ),
                                                                   _camera((float)width, (float)height),
                                                                   _currentFrameTime(0),
                                                                   _previousFrameTime(0),
                                                                   _dt(0)
                                                                   {
        glfwSetWindowUserPointer(_window.GetNativeWindow(), reinterpret_cast<void *>(&_camera)); // Allow access to camera through GLFW callbacks.
    }

    void Scene::Init() {
        OnInit();
    }

    void Scene::Run() {
        do {
            BeginFrame();

            OnUpdate(_dt);

            OnPreRender();
            OnRender();
            OnPostRender();

            OnImGui();

            EndFrame();
        }
        while (_window.IsActive());
    }

    void Scene::Shutdown() {
        OnShutdown();
    }

    Scene::~Scene() {
    }

    void Scene::BeginFrame() {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Calculate dt.
        _currentFrameTime = (float)glfwGetTime();
        _dt = _currentFrameTime - _previousFrameTime;
        _previousFrameTime = _currentFrameTime;

        ProcessInput();
    }

    void Scene::EndFrame() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        _window.SwapBuffers();
        _window.PollEvents();
    }

    void Scene::ProcessInput() {
        static bool showCursor = true;

        const float cameraSpeed = 5.0f;
        const glm::vec3& cameraPosition = _camera.GetEyePosition();
        const glm::vec3& cameraForwardVector = _camera.GetForwardVector();
        const glm::vec3& cameraUpVector = _camera.GetUpVector();

        // Move camera
        if (glfwGetKey(_window.GetNativeWindow(), GLFW_KEY_W) == GLFW_PRESS) {
            _camera.SetEyePosition(cameraPosition + cameraSpeed * cameraForwardVector * _dt);
        }

        if (glfwGetKey(_window.GetNativeWindow(), GLFW_KEY_S) == GLFW_PRESS) {
            _camera.SetEyePosition(cameraPosition - cameraSpeed * cameraForwardVector * _dt);
        }

        if (glfwGetKey(_window.GetNativeWindow(), GLFW_KEY_A) == GLFW_PRESS) {
            _camera.SetEyePosition(cameraPosition - glm::normalize(glm::cross(cameraForwardVector, cameraUpVector)) * cameraSpeed * _dt);
        }

        if (glfwGetKey(_window.GetNativeWindow(), GLFW_KEY_D) == GLFW_PRESS) {
            _camera.SetEyePosition(cameraPosition + glm::normalize(glm::cross(cameraForwardVector, cameraUpVector)) * cameraSpeed * _dt);
        }

        if (glfwGetKey(_window.GetNativeWindow(), GLFW_KEY_E) == GLFW_PRESS) {
            _camera.SetEyePosition(cameraPosition + cameraSpeed * cameraUpVector * _dt);
        }

        if (glfwGetKey(_window.GetNativeWindow(), GLFW_KEY_Q) == GLFW_PRESS) {
            _camera.SetEyePosition(cameraPosition - cameraSpeed * cameraUpVector * _dt);
        }
    }

}
