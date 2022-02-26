
#pragma once

#include "common/utility/log.h"

namespace Sandbox {

    template <typename T>
    void SceneManager::AddScene(const std::string& name) {
        static_assert(std::is_base_of_v<IScene, T>, "Scene types passed into SceneManager::AddScene must derive from IScene.");

        if (!ValidateSceneName(name)) {
            ImGuiLog::Instance().LogError("SceneManager::AddScene called with a scene name that already exists.");
            throw std::runtime_error("SceneManager::AddScene called with a scene name that already exists.");
        }

        scenes_.emplace_back(new SceneType<T>(name));
    }

    template <typename T>
    SceneManager::SceneType<T>::SceneType(const std::string& name) : ISceneType(name) {
    }

    template <typename T>
    SceneManager::SceneType<T>::~SceneType() {
        delete reinterpret_cast<T*>(scene_);
        scene_ = nullptr;
    }

    template <typename T>
    void SceneManager::SceneType<T>::Create() {
        if (scene_) {
            // Should never happen.
            ImGuiLog::Instance().LogError("SceneType::Create called on already instantiated SceneType.");
        }

        scene_ = new T();
    }

    template <typename T>
    void SceneManager::SceneType<T>::Destroy() {
        if (scene_) {
            delete reinterpret_cast<T*>(scene_);
        }
    }

}