
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
    	ImGuiLog& log = ImGuiLog::GetInstance();

        // Main menu bar (scene selection).
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Scenes")) {

            	int index = 0;

                for (const SceneData& data : scenes_) {
                    if (ImGui::MenuItem(data.prettyName_.c_str())) {
                    	if (index == currentIndex_) {
                    		// Selected scene is already active.
							continue;
                    	}

                    	UnloadSceneData();

                    	currentIndex_ = index;
                        LoadSceneData();
                    }

                    ++index;
                }
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();

        IScene* current = GetCurrentScene();

        // Run currently active scene.
        current->OnUpdate(dt);

        current->OnPreRender();
        current->OnRender();
        current->OnPostRender();

        current->OnImGui();
    }

    void SceneManager::Shutdown() {
    	IScene* current = GetCurrentScene();

    	current->OnShutdown(); // TODO: double delete if called directly after main menu bar?

        for (const SceneData& data : scenes_) {
            delete data.scene_;
        }
    }

    void SceneManager::AddScene(const std::string& name, IScene* scene) {
        scenes_.emplace_back(name, scene);
    }

    IScene* SceneManager::GetCurrentScene() const {

    }

    void SceneManager::LoadSceneData() const {
    	ImGuiLog& log = ImGuiLog::GetInstance();

		const SceneData& data = scenes_[currentIndex_];

		log.LogTrace("Loading scene: '%s'", data.prettyName_.c_str());

		auto start = std::chrono::high_resolution_clock::now();

		// Load any saved ImGui settings.
		if (std::filesystem::exists(data.imGuiIniPath_)) {
			log.LogTrace("Found existing ImGui layout file: %s", data.imGuiIniPath_.c_str());
			ImGui::LoadIniSettingsFromDisk(data.imGuiIniPath_.c_str());
		}
		else {
			log.LogTrace("Creating new ImGui layout file: %s", data.imGuiIniPath_.c_str());
		}

		IScene* current = data.scene_;
		current->OnInit();

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		log.LogTrace("Scene loaded in: %i ms.", duration.count());
    }

	const SceneManager::SceneData& SceneManager::GetCurrentSceneData() const {
    	if (currentIndex_ >= scenes_.size()) {
    		throw std::out_of_range("Invalid scene configuration.");
    	}

    	if (currentIndex_ < 0) {
    		return nullptr;
    	}

    	return scenes_[currentIndex_].scene_;
	}

	void SceneManager::UnloadSceneData() const {
    	IScene* current = GetCurrentScene();

    	// Shutdown current scene.
    	current->OnShutdown();

    	// Save ImGui settings.
    	const std::string& ini = data.imGuiIniPath_;
    	log.LogTrace("Saving ImGui settings to: %s", ini.c_str());
    	ImGui::SaveIniSettingsToDisk(ini.c_str());

    	// Save log file.
    	const std::string txt = data.imGuiLogPath_;
    	log.LogTrace("Saving ImGui log to: %s", txt.c_str());
    	log.WriteToFile(txt);
	}

	SceneManager::SceneData::SceneData(std::string sceneName, IScene* scene) : scene_(scene),
																			   prettyName_(std::move(sceneName))
																			   {
    	ImGuiLog& log = ImGuiLog::GetInstance();

    	// Convert name to all lowercase, with underscores instead of spaces.
    	std::stringstream builder;
    	for (char character : prettyName_) {
    		if (character == ' ') {
    			builder << '_';
    		}
    		else {
    			builder << static_cast<char>(std::tolower(character));
    		}
    	}
    	name_ = builder.str();

    	dataDirectory_ = "data/scenes/";

    	imGuiLogPath_ = dataDirectory_ + name_ + "log.txt";
    	imGuiIniPath_ = dataDirectory_ + name_ + "gui.ini";
	}

	SceneManager::SceneData::~SceneData() {
	}

}
