
#ifndef SANDBOX_COMPONENT_MANAGER_COLLECTION_TPP
#define SANDBOX_COMPONENT_MANAGER_COLLECTION_TPP

#include "pch.h"
#include "common/ecs/component/component.h"
#include "common/utility/helper.h"

namespace Sandbox {

    template <typename ...T>
    ComponentManagerCollection<T...>::ComponentManagerCollection() : typeIDCounter_(0) {
        // TODO: ensure parameter pack contains unique types.
        static_assert((std::is_base_of_v<IComponent, T> && ...), "Invalid template parameter provided to base BaseComponentSystem - component types must derive from IComponent.");
    }

    template <typename ...T>
    ComponentManagerCollection<T...>::~ComponentManagerCollection() {
    }

    template<typename... T>
    void ComponentManagerCollection<T...>::Init() {
        // Generate type mapping (types need to exist before their respective ComponentManager).
        PARAMETER_PACK_EXPAND(GenerateTypeID, T);

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
    ComponentManager<Y>* ComponentManagerCollection<T...>::GetComponentManager() const {
        static_assert(std::is_base_of_v<IComponent, Y>, "Template type Y provided to GetComponentManager must derive from IComponent.");
        static_assert((std::is_same_v<Y, T> || ...), "Template type Y provided to GetComponentManager must match one of the template types of the ComponentManagerCollection.");

        ComponentManager<Y>* componentManager = dynamic_cast<ComponentManager<Y>*>(componentManagers_.at(GetIDFromType<Y>())); // const.
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
        static_assert((std::is_same_v<Y, T> || ...), "Template type Y provided to CreateComponentManager must match one of the template types of the ComponentManagerCollection.");

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
        static_assert((std::is_same_v<Y, T> || ...), "Template type Y provided to ResetComponentManager must match one of the template types of the ComponentManagerCollection.");

        int componentID = GetIDFromType<Y>();
        ComponentManager<Y>* componentManager = dynamic_cast<ComponentManager<Y>*>(componentManagers_[componentID]);
        assert(componentManager); // Sanity.

        componentManager->Reset();
    }

    template <typename... T>
    template <typename Y>
    void ComponentManagerCollection<T...>::RemoveComponent(int entityID) {
        static_assert((std::is_same_v<Y, T> || ...), "Template type Y provided to RemoveComponent must match one of the template types of the ComponentManagerCollection.");

        int componentID = GetIDFromType<Y>();
        ComponentManager<Y>* componentManager = dynamic_cast<ComponentManager<Y>*>(componentManagers_[componentID]);
        assert(componentManager); // Sanity.

        componentManager->RemoveComponent(entityID);
    }

    template <typename ...T>
    template <typename Y>
    int ComponentManagerCollection<T...>::GetIDFromType() const {
        // Static asserts ensure type exists.
        std::type_index type = std::type_index(typeid(Y));
        assert(typeIDMapping_.find(type) != typeIDMapping_.end()); // Sanity.
        return typeIDMapping_.at(type);
    }

    template<typename... T>
    template<typename Y>
    void ComponentManagerCollection<T...>::GenerateTypeID() {
        std::type_index type = std::type_index(typeid(Y));

        auto iterator = typeIDMapping_.find(type);
        assert(iterator == typeIDMapping_.end());
        typeIDMapping_.emplace(type, typeIDCounter_++);
    }

}

#endif //SANDBOX_COMPONENT_MANAGER_COLLECTION_TPP
