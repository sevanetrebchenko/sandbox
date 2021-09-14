
#ifndef SANDBOX_SCENE_MANAGER_TPP
#define SANDBOX_SCENE_MANAGER_TPP

#include <framework/imgui_log.h>

namespace Sandbox {

    template <typename SceneType, typename ...Args>
    void SceneManager::RegisterScene(const std::string& sceneName, const std::string& scenePath, Args&& ...args) {
        static_assert(std::is_base_of<IScene, SceneType>::value, "Scenes must be derived from IScene.");

        ImGuiLog* log = Singleton<ImGuiLog>::GetInstance();

        if (scenePath.empty()) {
            log->LogError("Scene path provided to RegisterScene is empty.");
            throw std::runtime_error("Scene path provided to RegisterScene is empty.");
        }

        if (sceneName.empty()) {
            log->LogError("Scene name provided to RegisterScene is empty.");
            throw std::runtime_error("Scene name provided to RegisterScene is empty.");
        }

        // Ensure scene has not already been registered.
        for (const SceneData& data : scenes_) {
            if (sceneName == data.scenePrefix_) {
                log->LogError("RegisterScene called with already existing scene.");
                throw std::runtime_error("RegisterScene called with already existing scene.");
            }
        }

        Window* window = Singleton<Window>::GetInstance();

        SceneData data;
        SceneType* scene = new SceneType(std::forward<Args>(args)...);
        scene->sceneData_ = &data;
        scene->camera_.SetWidth(window->GetWidth());
        scene->camera_.SetHeight(window->GetHeight());
        data.scene_ = scene;

        // Convert scene name to lowercase.
        std::stringstream stringbuilder;
        for (char character : sceneName) {
            // Convert spaces to underscores.
            if (character == ' ') {
                stringbuilder << '_';
            }
            else {
                stringbuilder << static_cast<char>(std::tolower(character));
            }
        }
        data.scenePrefix_ = stringbuilder.str();
        data.sceneName_ = sceneName;
        data.dataDirectory_ = NativePathConverter::ConvertToNativeSeparators(scenePath);
        data.imGuiIniName_ = data.scenePrefix_ + "_imgui.ini";
        data.imGuiLogName_ = data.scenePrefix_ + "_log.txt";

        log->LogTrace("Registered scene '%s' with data directory '%s'", sceneName.c_str(), scenePath.c_str());

        scenes_.push_back(data);
    }

}

#endif //SANDBOX_SCENE_MANAGER_TPP