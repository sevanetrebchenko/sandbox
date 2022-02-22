
#include "common/application/scene_manager.h"
#include "common/utility/log.h"

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

                for (const SceneData& data : scenes_) {
                    if (ImGui::MenuItem(data.prettyName_.c_str())) {
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
        const std::string file = "data/log.txt";
        log.LogTrace("Saving ImGui log to: %s", file.c_str());
        log.WriteToFile(file);

        for (const SceneData& data : scenes_) {
            delete data.scene_;
        }
    }

    void SceneManager::AddScene(const std::string& name, IScene* scene) {
        scenes_.emplace_back(name, scene);
    }

    IScene* SceneManager::GetCurrentScene() const {
        const SceneData* data = GetCurrentSceneData();

        if (!data) {
            return nullptr;
        }

        return data->scene_;
    }

    void SceneManager::SetStartupScene(const std::string& name) {
        if (currentIndex_ != -1) {
            // Function only does something when no scene has been loaded.
            return;
        }

        ImGuiLog& log = ImGuiLog::Instance();

        // Find scene data with given scene name.
        for (int i = 0; i < scenes_.size(); ++i) {
            const SceneData& data = scenes_[i];
            const std::string& sceneName = data.prettyName_;

            if (sceneName == name) {
                log.LogTrace("Setting startup scene as: '%s'", sceneName.c_str());
                currentIndex_ = i;
                break;
            }
        }
    }

    void SceneManager::LoadSceneData() const {
    	ImGuiLog& log = ImGuiLog::Instance();

		const SceneData* data = GetCurrentSceneData();
        if (!data) {
            return;
        }

		log.LogTrace("Loading scene: '%s'", data->prettyName_.c_str());

		auto start = std::chrono::high_resolution_clock::now();

		// Load any saved ImGui settings.
		if (std::filesystem::exists(data->imGuiIniPath_)) {
			log.LogTrace("Found existing ImGui layout file: %s", data->imGuiIniPath_.c_str());
			ImGui::LoadIniSettingsFromDisk(data->imGuiIniPath_.c_str());
		}
		else {
			log.LogTrace("Creating new ImGui layout file: %s", data->imGuiIniPath_.c_str());
		}

        // Enable manual save settings.
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;

		IScene* current = data->scene_;
		current->OnInit();

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		log.LogTrace("Scene loaded in: %i ms.", duration.count());
    }

	const SceneManager::SceneData* SceneManager::GetCurrentSceneData() const {
        // Current index can be negative (no active scenes).
        if (currentIndex_ < 0) {
            return nullptr;
        }

    	if (currentIndex_ >= scenes_.size()) {
    		throw std::out_of_range("Invalid SceneManager configuration.");
    	}

    	return &scenes_[currentIndex_];
	}

	void SceneManager::UnloadSceneData() const {
        ImGuiLog& log = ImGuiLog::Instance();
    	const SceneData* data = GetCurrentSceneData();

        if (!data) {
            return;
        }

        log.LogTrace("Unloading scene: '%s'", data->prettyName_.c_str());

        auto start = std::chrono::high_resolution_clock::now();

        // Shutdown current scene.
        IScene* scene = data->scene_;
        scene->OnShutdown();

        // TODO: Create directory if it doesn't exist.

    	// Save ImGui settings.
    	const std::string& ini = data->imGuiIniPath_;
    	log.LogTrace("Saving ImGui settings to: %s", ini.c_str());
    	ImGui::SaveIniSettingsToDisk(ini.c_str());

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

    SceneManager::SceneData::SceneData(std::string sceneName, IScene* scene) : scene_(scene),
																			   prettyName_(std::move(sceneName))
																			   {
    	ImGuiLog& log = ImGuiLog::Instance();

    	// Convert name to all lowercase, with underscores instead of spaces.
    	std::stringstream builder;
    	for (char character : prettyName_) {
    		if (character == ' ') {
    			builder << '_';
    		}
            else if (character == ':') {
                continue;
            }
    		else {
    			builder << static_cast<char>(std::tolower(character));
    		}
    	}
    	name_ = builder.str();
    	imGuiIniPath_ = "data/scenes/" + name_ + "/" + name_ + "_gui.ini";
	}

	SceneManager::SceneData::~SceneData() {
	}

}
