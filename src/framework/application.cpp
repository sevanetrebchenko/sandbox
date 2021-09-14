
#include <framework/application.h>
#include <framework/texture_library.h>

namespace Sandbox {

    Application::Application() : dt_(0.0f),
                                 currentFrameTime_(0.0f),
                                 previousFrameTime_(0.0f)
                                 {
        // Initialize singleton data.
        Singleton<ImGuiLog>::GetInstance()->Initialize();

        Singleton<Window>::GetInstance()->Initialize(); // Window initializes OpenGL context.
        Singleton<OBJLoader>::GetInstance()->Initialize();
        Singleton<ModelManager>::GetInstance()->Initialize();
        Singleton<LightingManager>::GetInstance()->Initialize();
        Singleton<ShaderLibrary>::GetInstance()->Initialize();
        Singleton<MaterialLibrary>::GetInstance()->Initialize();
        Singleton<TextureLibrary>::GetInstance()->Initialize();
    }

    Application::~Application() {
        // Shutdown all singletons in reverse order.
        Singleton<TextureLibrary>::GetInstance()->Shutdown();
        Singleton<MaterialLibrary>::GetInstance()->Shutdown();
        Singleton<ShaderLibrary>::GetInstance()->Shutdown();
        Singleton<LightingManager>::GetInstance()->Shutdown();
        Singleton<ModelManager>::GetInstance()->Shutdown();
        Singleton<OBJLoader>::GetInstance()->Shutdown();
        Singleton<Window>::GetInstance()->Shutdown();
        Singleton<ImGuiLog>::GetInstance()->Shutdown();
    }

    void Application::Initialize() {
        IScene* currentScene = sceneManager_.GetCurrentScene();
        currentScene->OnInit();
    }

    void Application::Run() {
        Window* window = Singleton<Window>::GetInstance();

        do {
            IScene* currentScene = sceneManager_.GetCurrentScene();

            BeginFrame();

            Update();
            currentScene->OnUpdate(dt_);

            currentScene->OnPreRender();
            currentScene->OnRender();
            currentScene->OnPostRender();

            OnImGui();
            currentScene->OnImGui();

            EndFrame();
        }
        while (window->IsActive());
    }

    void Application::Shutdown() {
    }

    void Application::BeginFrame() {
        // Start the ImGui frame.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Calculate dt.
        currentFrameTime_ = (float) glfwGetTime();
        dt_ = currentFrameTime_ - previousFrameTime_;
        previousFrameTime_ = currentFrameTime_;
    }

    void Application::Update() {
        ShaderLibrary* shaderLibrary = Singleton<ShaderLibrary>::GetInstance();
        LightingManager* lightingManager = Singleton<LightingManager>::GetInstance();
        ModelManager* modelManager = Singleton<ModelManager>::GetInstance();
        ImGuiLog* log = Singleton<ImGuiLog>::GetInstance();

        lightingManager->Update();
        for (Model* model : modelManager->GetModels()) {
            model->Update();
        }

        // Recompile shaders.
        try {
            shaderLibrary->RecompileAllModified();
        }
        catch (std::runtime_error& err) {
            log->LogError("Shader recompilation failed: %s", err.what());
        }

        ProcessInput();
    }

    void Application::OnImGui() {
        // Enable window docking.
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // Framework overview.
        if (ImGui::Begin("Overview")) {
            ImGui::Text("Render time:");
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        ImGui::End();

        sceneManager_.OnImGui();
    }

    void Application::EndFrame() {
        Window* window = Singleton<Window>::GetInstance();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window->SwapBuffers();
        window->PollEvents();
    }

    void Application::ProcessInput() {
        static bool showCursor = true;

        IScene* currentScene = sceneManager_.GetCurrentScene();
        Camera& camera = currentScene->camera_;
        Window* window = Singleton<Window>::GetInstance();

        const float cameraSpeed = 5.0f;
        const glm::vec3& cameraPosition = camera.GetEyePosition();
        const glm::vec3& cameraForwardVector = camera.GetForwardVector();
        const glm::vec3& cameraUpVector = camera.GetUpVector();

        // Move camera
        if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_W) == GLFW_PRESS) {
            camera.SetEyePosition(cameraPosition + cameraSpeed * cameraForwardVector * dt_);
        }

        if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_S) == GLFW_PRESS) {
            camera.SetEyePosition(cameraPosition - cameraSpeed * cameraForwardVector * dt_);
        }

        if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_A) == GLFW_PRESS) {
            camera.SetEyePosition(cameraPosition - glm::normalize(glm::cross(cameraForwardVector, cameraUpVector)) * cameraSpeed * dt_);
        }

        if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_D) == GLFW_PRESS) {
            camera.SetEyePosition(cameraPosition + glm::normalize(glm::cross(cameraForwardVector, cameraUpVector)) * cameraSpeed * dt_);
        }

        if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_E) == GLFW_PRESS) {
            camera.SetEyePosition(cameraPosition + cameraSpeed * cameraUpVector * dt_);
        }

        if (glfwGetKey(window->GetNativeWindow(), GLFW_KEY_Q) == GLFW_PRESS) {
            camera.SetEyePosition(cameraPosition - cameraSpeed * cameraUpVector * dt_);
        }
    }

    SceneManager* Application::GetSceneManager() {
        return &sceneManager_;
    }

}
