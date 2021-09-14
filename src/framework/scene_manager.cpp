
#include <framework/scene_manager.h>
#include <framework/imgui_log.h>
#include <framework/lighting_manager.h>
#include <framework/material_library.h>
#include <framework/shader_library.h>
#include <framework/model_manager.h>
#include <framework/texture_library.h>

namespace Sandbox {

    SceneManager::SceneManager() : initialized_(false),
                                   sceneLoaded_(false)
                                   {
    }

    SceneManager::~SceneManager() {
    }

    void SceneManager::LoadSceneData() {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiLog* log = Singleton<ImGuiLog>::GetInstance();

        // In case no startup scene was selected.
        if (!initialized_) {
            current_ = scenes_.begin();
            initialized_ = true;
        }

        bool foundIni = false;
        bool foundTxt = false;

        const SceneData& sceneData = *current_;
        IScene* scene = sceneData.scene_;

        scene->camera_.Default();
        glfwSetWindowUserPointer(Singleton<Window>::GetInstance()->GetNativeWindow(), reinterpret_cast<void *>(&scene->camera_)); // Allow access to camera through GLFW callbacks.

        // Check if directory contains .ini file. If so, use that instead.
        std::vector<std::string> filesInDirectory = DirectoryReader::GetFiles(sceneData.dataDirectory_);
        for (std::string& file : filesInDirectory) {
            std::string assetExtension = DirectoryReader::GetAssetExtension(file);
            std::string assetName = DirectoryReader::GetAssetName(file);

            // Found file, use this file instead.
            if (assetExtension == "ini" && assetName == sceneData.imGuiIniName_) {
                log->LogTrace("Found existing ImGui layout (%s).", file.c_str());

                ImGui::LoadIniSettingsFromDisk(file.c_str());
                foundIni = true;
            }
            else if (assetExtension == "txt" && assetName == sceneData.imGuiLogName_) {
                log->LogTrace("Found existing ImGui log file (%s).", file.c_str());

                foundTxt = true;
            }
        }

        // Enable manual Imgui .ini save settings.
        io.IniFilename = nullptr;

        if (foundIni && foundTxt) {
            return;
        }

        // Create new files.
        if (!foundIni) {
            log->LogTrace("Creating new ImGui layout in location (%s).", sceneData.imGuiIniName_.c_str());
        }

        if (!foundTxt) {
            log->LogTrace("Creating new ImGui log in location (%s).", sceneData.imGuiLogName_.c_str());
        }
    }

    void SceneManager::OnImGui() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Menu")) {
                for (const SceneData& sceneData : scenes_) {
                    if (ImGui::MenuItem(sceneData.sceneName_.c_str())) {
                        SwitchScene(sceneData.sceneName_);
                    }
                }

                ImGui::EndMenu();
            }
        }

        ImGui::EndMainMenuBar();

        ImGuiIO& io = ImGui::GetIO();
        const SceneData& sceneData = *current_;

        if (io.WantSaveIniSettings) {
            const SceneData& data = *current_;
            std::string outputFilePath = NativePathConverter::ConvertToNativeSeparators(data.dataDirectory_ + "/" + data.imGuiIniName_);
            ImGui::SaveIniSettingsToDisk(outputFilePath.c_str());

            // Manually change flag.
            io.WantSaveIniSettings = false;
        }
    }

    void SceneManager::SwitchScene(const std::string &sceneName) {
        ImGuiLog* log = Singleton<ImGuiLog>::GetInstance();

        // Shutdown old scene.
        const SceneData& data = *current_;
        IScene* currentScene = data.scene_;

        if (data.sceneName_ == sceneName) {
            // Don't switch if requested scene is the same as this.
            return;
        }

        currentScene->OnShutdown();

        // Clear all managers.
        Singleton<LightingManager>::GetInstance()->Clear();
        Singleton<MaterialLibrary>::GetInstance()->Clear();
        Singleton<ModelManager>::GetInstance()->Clear();
        Singleton<ShaderLibrary>::GetInstance()->Clear();

        // Output log.
        {
            std::string outputFilePath = NativePathConverter::ConvertToNativeSeparators(data.dataDirectory_ + "/" + data.imGuiLogName_);
            log->WriteToFile(outputFilePath);
        }

        // Output ImGui .ini file.
        {
            std::string outputFilePath = NativePathConverter::ConvertToNativeSeparators(data.dataDirectory_ + "/" + data.imGuiIniName_);
            ImGui::SaveIniSettingsToDisk(outputFilePath.c_str());
        }

        bool found = false;
        std::list<SceneData>::iterator sceneIterator;

        for (sceneIterator = scenes_.begin(); sceneIterator != scenes_.end(); ++sceneIterator) {
            const SceneData& sceneData = *sceneIterator;
            if (sceneData.sceneName_ == sceneName) {
                found = true;
                break;
            }
        }

        if (!found) {
            log->LogError("SwitchScene called for unregistered scene name '%s'.", sceneName.c_str());
            throw std::runtime_error("SwitchScene called for unregistered scene name.");
        }

        log->LogTrace("Switching to scene '%s'.", sceneName.c_str());
        IScene* desired = sceneIterator->scene_;
        current_ = sceneIterator;

        LoadSceneData();

        desired->OnInit();
    }

    IScene *SceneManager::GetCurrentScene() {
        if (current_ == scenes_.end()) {
            return nullptr;
        }

        // Make sure first scene is loaded on request.
        if (!sceneLoaded_) {
            LoadSceneData();
            sceneLoaded_ = true;
        }

        return current_->scene_;
    }

    void SceneManager::SetStartupScene(const std::string &sceneName) {
        ImGuiLog* log = Singleton<ImGuiLog>::GetInstance();

        bool found = false;
        std::list<SceneData>::iterator sceneIterator;

        for (sceneIterator = scenes_.begin(); sceneIterator != scenes_.end(); ++sceneIterator) {
            const SceneData& sceneData = *sceneIterator;
            if (sceneData.sceneName_ == sceneName) {
                found = true;
                break;
            }
        }

        if (!found) {
            log->LogError("SetStartupScene called for unregistered scene name '%s'.", sceneName.c_str());
            throw std::runtime_error("SetStartupScene called for unregistered scene name.");
        }

        current_ = sceneIterator;
        initialized_ = true;
    }

}
