
#include <framework/scene.h>
#include <framework/imgui_log.h>

namespace Sandbox {

    Scene::Scene(std::string name, int width, int height) : _window( { "Sandbox", width, height } ),
                                                            _camera((float)width, (float)height),
                                                            _currentFrameTime(0),
                                                            _previousFrameTime(0),
                                                            _dt(0),
                                                            _sceneName(std::move(name))
                                                            {
        glfwSetWindowUserPointer(_window.GetNativeWindow(), reinterpret_cast<void *>(&_camera)); // Allow access to camera through GLFW callbacks.
    }

    void Scene::Init() {
        LoadSceneData();
        OnInit();
    }

    void Scene::Run() {
        do {
            BeginFrame();

            Update(_dt);
            OnUpdate(_dt);

            OnPreRender();
            OnRender();
            OnPostRender();

            OnImGuiMenu();
            OnImGui();

            EndFrame();
        }
        while (_window.IsActive());
    }

    void Scene::Shutdown() {
        OnShutdown();

        // Save ImGui settings.
        ImGuiLog::GetInstance().LogTrace("Saving ImGui settings to: %s", _imGuiIniName.c_str());
        ImGui::SaveIniSettingsToDisk(_imGuiIniName.c_str());

        ImGuiLog& log = ImGuiLog::GetInstance();
        log.LogTrace("Saving ImGui log to: %s", _imGuiLogName.c_str());
        log.WriteToFile(_imGuiLogName);
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

    void Scene::Update(float dt) {
        _modelManager.Update(dt);
        _lightingManager.Update();
    }

    void Scene::EndFrame() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Save ImGui .ini settings.
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantSaveIniSettings) {
            ImGui::SaveIniSettingsToDisk(_imGuiIniName.c_str());

            // Manually change flag.
            io.WantSaveIniSettings = false;
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

    void Scene::LoadSceneData() {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiLog& log = ImGuiLog::GetInstance();

        // Convert name to lowercase.
        std::stringstream stringbuilder;
        for (char character : _sceneName) {
            // Convert spaces to underscores.
            if (character == ' ') {
                stringbuilder << '_';
            }
            else {
                stringbuilder << static_cast<char>(std::tolower(character));
            }
        }
        std::string sceneNameLowercase = stringbuilder.str();

        _dataDirectory = NativePathConverter::ConvertToNativeSeparators(_dataDirectory);
        if (!_dataDirectory.empty()) {
            // If the last character is not a native /, append it.
            const char& lastChar = _dataDirectory.back();
            char native;
#ifdef _WIN32
            native = '\\';
#else
            native = '/';
#endif

            if (lastChar != native) {
                _dataDirectory += native;
            }
        }

        bool foundIni = false;
        bool foundTxt = false;

        // Check if directory contains .ini file. If so, use that instead.
        std::vector<std::string> filesInDirectory = DirectoryReader::GetFiles(_dataDirectory);
        for (std::string& file : filesInDirectory) {
            std::string assetExtension = DirectoryReader::GetAssetExtension(file);
            std::string assetName = DirectoryReader::GetAssetName(file);

            // Found file, use this file instead.
            if (assetExtension == "ini") {
                log.LogTrace("Found existing ImGui layout (%s).", file.c_str());

                _imGuiIniName = file;
                ImGui::LoadIniSettingsFromDisk(file.c_str());
                foundIni = true;
            }
            else if (assetExtension == "txt" && assetName == sceneNameLowercase) {
                log.LogTrace("Found existing ImGui log file (%s).", file.c_str());

                _imGuiLogName = sceneNameLowercase + ".txt";
                foundTxt = true;
            }
        }

        if (foundIni && foundTxt) {
            return;
        }

        // Create new files.
        if (!foundIni) {
            _imGuiIniName = _dataDirectory + sceneNameLowercase + "_imgui.ini"; // .ini path needs to be available for the duration of program lifetime.
            log.LogTrace("Creating new ImGui layout in location (%s).", _imGuiIniName.c_str());
        }

        // Enable manual Imgui .ini save settings.
        io.IniFilename = nullptr;

        if (!foundTxt) {
            _imGuiLogName = _dataDirectory + sceneNameLowercase + "_log.txt";
            log.LogTrace("Creating new ImGui log in location (%s).", _imGuiLogName.c_str());
        }
    }

    void Scene::OnImGuiMenu() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Menu"))
            {
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();
    }

}
