
#pragma once

#include "pch.h"
#include "common/ecs/component/component.h"

namespace Sandbox {

    // Has no ownership of components.
    // Represents the active components attached to an entity AT THE TIME of construction.
    class ComponentList {
        public:
            // First time using this...
            template <typename ...T>
            explicit ComponentList(T*... components);

            // Manual configuration.
            // No public type safety for this constructor, guaranteed to be safe when called from ECS internals.
            explicit ComponentList(const std::unordered_map<std::type_index, IComponent*>& componentPairing);
            ComponentList() = delete;

            ~ComponentList();

            template <typename T>
            [[nodiscard]] T* GetComponent() const;

            template <typename T>
            [[nodiscard]] bool HasComponent() const;

        private:
            template <typename T>
            void AddComponent(T* component);

            void AddComponent(std::type_index type, IComponent* component);

            std::unordered_map<std::type_index, IComponent*> componentMapping_;
    };

}

#include "common/ecs/component/component_list.tpp"