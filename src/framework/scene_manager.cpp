
#include <framework/scene_manager.h>
#include <framework/imgui_log.h>

namespace Sandbox {

    SceneManager::SceneManager() {
    }

    SceneManager::~SceneManager() {
    }

    void SceneManager::Init() {
    }

    void SceneManager::Update(float dt) {
        // Main menu bar (scene selection).
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Scenes")) {

                for (const std::pair<const std::string, IScene*>& data : scenes_) {
                    const std::string& name = data.first;
                    IScene* scene = data.second;

                    if (ImGui::MenuItem(name.c_str())) {
                        // Shutdown current scene.
                        current_.scene_->OnShutdown();

                        // Load selected scene backend data.
                        currentScene_ = scene;
                        LoadSceneData();

                        currentScene_->OnInit();
                    }
                }

                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();

        // Run currently active scene.
        currentScene_->OnUpdate(dt);

        currentScene_->OnPreRender();
        currentScene_->OnRender();
        currentScene_->OnPostRender();

        currentScene_->OnImGui();
    }

    void SceneManager::Shutdown() {
        currentScene_->OnShutdown(); // TODO: double delete if called directly after main menu bar?
        currentScene_ = nullptr;

        for (const std::pair<const std::string, IScene*>& data : scenes_) {
            delete data.second;
        }
    }

    void SceneManager::AddScene(const std::string& name, IScene* scene) {


        scenes_.emplace(name, scene);
    }

    IScene* SceneManager::GetCurrentScene() const {
        return currentScene_;
    }

    void SceneManager::LoadSceneData() const {
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

}
