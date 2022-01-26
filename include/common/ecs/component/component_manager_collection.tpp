
#ifndef SANDBOX_COMPONENT_MANAGER_COLLECTION_TPP
#define SANDBOX_COMPONENT_MANAGER_COLLECTION_TPP

#include "pch.h"
#include "common/ecs/component/component.h"

#define PARAMETER_PACK_EXPAND(function, args, ...) ((void)function<args>(__VA_ARGS__), ...);

namespace Sandbox {

    template <typename ...T>
    ComponentManagerCollection<T...>::ComponentManagerCollection() : typeIDCounter_(0) {
        static_assert((std::is_base_of_v<IComponent, T> && ...), "Invalid template parameter provided to base BaseComponentSystem - component types must derive from IComponent.");
    }

    template <typename ...T>
    ComponentManagerCollection<T...>::~ComponentManagerCollection() {
    }

    template<typename... T>
    void ComponentManagerCollection<T...>::Init() {
        // Create a ComponentManager for each type of component.
        PARAMETER_PACK_EXPAND(CreateComponentManager, T);
    }

    template<typename... T>
    void ComponentManagerCollection<T...>::Reset() {
        // Clear all components from registered ComponentManagers.
        PARAMETER_PACK_EXPAND(ResetComponentManager, T);
    }

    template<typename... T>
    void ComponentManagerCollection<T...>::Shutdown() {
        Reset();

        // Delete all component managers.
        for (const std::pair<const int, IComponentManager*>& data : componentManagers_) {
            delete data.second;
        }
    }

    template <typename ...T>
    template <typename Y>
    ComponentManager<Y>* ComponentManagerCollection<T...>::GetComponentManager() {
        static_assert(std::is_base_of_v<IComponent, Y>, "Template type Y provided to GetComponentManager must derive from IComponent.");

        ComponentManager<Y>* componentManager = dynamic_cast<ComponentManager<Y>*>(componentManagers_[GetIDFromType<Y>()]);
        assert(componentManager); // Should always succeed.

        return componentManager;
    }

    template<typename... T>
    void ComponentManagerCollection<T...>::RemoveAllComponents(int entityID) {
        PARAMETER_PACK_EXPAND(RemoveComponent, T, entityID);
    }

    template <typename... T>
    template <typename Y>
    void ComponentManagerCollection<T...>::CreateComponentManager() {
        int componentID = GetIDFromType<Y>();
        auto iterator = componentManagers_.find(componentID);
        assert(iterator == componentManagers_.end());
        componentManagers_.emplace(componentID, new ComponentManager<Y>());

        // Ensure component manager validity.
        ComponentManager<Y>* componentManager = dynamic_cast<ComponentManager<Y>*>(componentManagers_[componentID]);
        assert(componentManager);
    }

    template<typename... T>
    template<typename Y>
    void ComponentManagerCollection<T...>::ResetComponentManager() {
        int componentID = GetIDFromType<Y>();
        ComponentManager<Y>* componentManager = dynamic_cast<ComponentManager<Y>*>(componentManagers_[componentID]);
        assert(componentManager); // Sanity.

        componentManager->Reset();
    }

    template <typename... T>
    template <typename Y>
    void ComponentManagerCollection<T...>::RemoveComponent(int entityID) {
        int componentID = GetIDFromType<Y>();
        ComponentManager<Y>* componentManager = dynamic_cast<ComponentManager<Y>*>(componentManagers_[componentID]);
        assert(componentManager); // Sanity.

        componentManager->RemoveComponent(entityID);
    }

    template <typename ...T>
    template <typename Y>
    int ComponentManagerCollection<T...>::GetIDFromType() {
        std::type_index type = std::type_index(typeid(Y));

        auto iterator = typeIDMapping_.find(type);
        if (iterator == typeIDMapping_.end()) {
            // Type does not exist.
            typeIDMapping_.emplace(type, typeIDCounter_++);
        }

        return typeIDMapping_[type];
    }

}

#endif //SANDBOX_COMPONENT_MANAGER_COLLECTION_TPP
