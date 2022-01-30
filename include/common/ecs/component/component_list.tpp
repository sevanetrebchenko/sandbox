
#pragma once

namespace Sandbox {

    template <typename ...T>
    ComponentList::ComponentList(T*... components) {
        static_assert((std::is_base_of_v<IComponent, T> && ...), "Template type T provided to ComponentList must derive from IComponent.");

        // Register all types of components.
        (AddComponent<T>(components), ...);
    }

    template <typename T>
    T* ComponentList::GetComponent() const {
        auto iterator = componentMapping_.find(std::type_index(typeid(T)));
        if (iterator != componentMapping_.end()) {
            T* component = dynamic_cast<T*>(iterator->second);
            assert(component); // Sanity.
            return component;
        }
        else {
            return nullptr;
        }
    }

    template <typename T>
    bool ComponentList::HasComponent() const {
        auto iterator = componentMapping_.find(std::type_index(typeid(T)));
        if (iterator != componentMapping_.end()) {
            assert(dynamic_cast<T*>(iterator->second)); // Need to check, in case manual configuration constructor was erroneous.
            return true;
        }
        else {
            return false;
        }
    }

    template <typename T>
    void ComponentList::AddComponent(T* component) {
        std::type_index type = std::type_index(typeid(T));
        auto iterator = componentMapping_.find(type);

        if (iterator == componentMapping_.end()) {
            // Component does not yet exist, register.
            componentMapping_[type] = component;
        }
        else {
            // Attempting to override already existing component is an error.
            throw std::runtime_error("From ComponentList::AddComponent: attempting to override an already existing component.");
        }
    }

}