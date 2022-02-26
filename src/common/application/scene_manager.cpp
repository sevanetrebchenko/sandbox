
#include "common/application/scene_manager.h"
#include "common/utility/log.h"
#include "common/utility/directory.h"

namespace Sandbox {

    SceneManager::SceneManager() : previousIndex_(-1),
                                   currentIndex_(-1),
                                   sceneChangeRequested_(false)
                                   {
    }

    SceneManager::~SceneManager() {
    }

    void SceneManager::Init() {
    }

    void SceneManager::Update() {
    	ImGuiLog& log = ImGuiLog::Instance();

        // Main menu bar (scene selection).
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Scenes")) {

            	int index = 0;

                for (ISceneType* type : scenes_) {
                    assert(!type); // Should never happen.

                    if (ImGui::MenuItem(type->name_.c_str())) {
                    	currentIndex_ = index;
                    }

                    ++index;
                }
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();

        if (previousIndex_ != currentIndex_) {
            sceneChangeRequested_ = true;
        }
    }

    void SceneManager::Shutdown() {
        ImGuiLog& log = ImGuiLog::Instance();

        UnloadSceneData();

        // Save log file.
        const std::string file = ConvertToNativeSeparators(GetWorkingDirectory() + "/out/log.txt");
        log.LogTrace("Saving ImGui log to: %s", file.c_str());
        log.WriteToFile(file);
    }

    IScene* SceneManager::GetActiveScene() const {
        ISceneType* current = GetActiveSceneType();
        return current ? current->scene_ : nullptr;
    }

    void SceneManager::SetActiveScene(const std::string& name) {
        if (currentIndex_ != -1) {
            // Function only does something when no scene has been loaded.
            return;
        }

        ImGuiLog& log = ImGuiLog::Instance();

        // Find scene data with given scene name.
        for (int i = 0; i < scenes_.size(); ++i) {
            ISceneType* type = scenes_[i];
            const std::string& sceneName = type->name_;

            if (sceneName == name) {
                log.LogTrace("Setting startup scene as: '%s'", sceneName.c_str());
                currentIndex_ = i;
                break;
            }
        }
    }

    bool SceneManager::ValidateSceneName(const std::string& name) const {
        for (ISceneType* type : scenes_) {
            if (type->name_ == name) {
                return false;
            }
        }

        return true;
    }

    SceneManager::ISceneType* SceneManager::GetActiveSceneType() const {
        // Current index can be negative (no active scenes).
        if (currentIndex_ < 0) {
            return nullptr;
        }

        if (currentIndex_ >= scenes_.size()) {
            throw std::out_of_range("In SceneManager::GetActiveSceneType, invalid SceneManager configuration.");
        }

        return scenes_[currentIndex_];
    }

    void SceneManager::LoadSceneData() const {
    	ImGuiLog& log = ImGuiLog::Instance();

        ISceneType* type = GetActiveSceneType();
        if (!type) {
            // No active scene to load.
            return;
        }

        auto start = std::chrono::high_resolution_clock::now();

        type->Create(); // Create instance of scene.

        IScene* scene = type->scene_;

        std::string name = scene->GetName();
        if (name.empty()) {
            // Name was not set in the constructor of the scene.
            // Set the name to be the one the scene was registered in the SceneManager with.
            name = type->name_;
            scene->SetName(name);
        }

        scene->Lock(); // No more changes to scene name / data directory after this.
        scene->OnInit();

        log.LogTrace("Loading scene: '%s'", name.c_str());

        // Enable manual save settings.
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;

		// Load saved ImGui settings.
        const std::string& uiLayout = scene->GetUILayoutFile();
        if (std::filesystem::file_size({ uiLayout }) > 0) {
            // Load from .ini file only if it exists.
            log.LogTrace("Loading ImGui layout file: %s", uiLayout.c_str());
            ImGui::LoadIniSettingsFromDisk(uiLayout.c_str());
        }
        else {
            log.LogTrace("Creating ImGui layout file: %s", uiLayout.c_str());
        }

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		log.LogTrace("Scene loaded in: %i ms.", duration.count());
    }

	void SceneManager::UnloadSceneData() const {
        ImGuiLog& log = ImGuiLog::Instance();

        ISceneType* type = GetActiveSceneType();
        if (!type) {
            // No active scene to load.
            return;
        }

        auto start = std::chrono::high_resolution_clock::now();

        IScene* scene = type->scene_;
        assert(scene);

        const std::string& name = scene->GetName();

        log.LogTrace("Unloading scene: '%s'", name.c_str());

    	// Save ImGui settings.
    	const std::string& ini = scene->GetUILayoutFile();
    	log.LogTrace("Saving ImGui settings to: %s", ini.c_str());
    	ImGui::SaveIniSettingsToDisk(ini.c_str());

        // Shutdown current scene.
        scene->OnShutdown();
        type->Destroy();

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        log.LogTrace("Scene unloaded in: %i ms.", duration.count());
	}

	bool SceneManager::SceneChangeRequested() const {
		return sceneChangeRequested_;
	}

    void SceneManager::SwitchScenes() {
        if (previousIndex_ != -1) {
            // Unload previous scene data.
            UnloadSceneData();
        }

        // Load requested scene data.
        LoadSceneData();
        sceneChangeRequested_ = false;
        previousIndex_ = currentIndex_;
    }

    SceneManager::ISceneType::ISceneType(const std::string& name) : scene_(nullptr),
                                                                    name_(name)
                                                                    {
    }

    SceneManager::ISceneType::~ISceneType() = default;

}
