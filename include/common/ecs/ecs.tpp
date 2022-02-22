
#pragma once

#include "common/ecs/component/component.h"
#include "common/utility/log.h"

namespace Sandbox {

    // By entity ID.
    template <typename T, typename ...Args>
    ComponentWrapper<T> ECS::AddComponent(int entityID, const Args&... args) {
        T* component;

        if (HasComponentManager<T>()) {
            component = GetComponentManager<T>()->template AddComponent(entityID, args...);
        }
        else {
            // ComponentManager for requested type does not exist, and must be created first.
            component = AddComponentManager<T>()->template AddComponent(entityID, args...);
        }

        refreshSystems_ = true;
        DistributeECSEvent(entityID, ECSAction::COMPONENT_ADD);

        return ComponentWrapper<T>(component);
    }

    template <typename T>
    bool ECS::HasComponent(int entityID) const {
        // Short circuit.
        return HasComponentManager<T>() && GetComponentManager<T>()->HasComponent(entityID);
    }

    template<typename... T>
    bool ECS::HasComponents(int entityID) const {
        return (HasComponent<T>(entityID) && ...);
    }

    template <typename T>
    ComponentWrapper<T> ECS::GetComponent(int entityID) const {
        if (HasComponentManager<T>()) {
            return ComponentWrapper<T>(GetComponentManager<T>()->GetComponent(entityID));
        }
        else {
            // No registered component manager, means no entities have that component type.
            return ComponentWrapper<T>();
        }
    }

    template <typename T1, typename T2, typename ...Rest>
    ComponentList ECS::GetComponents(int entityID) const {
        // Use template deduction to call templatized constructor.
        return ComponentList(GetComponent<T1>(entityID), GetComponent<T2>(entityID), GetComponent<Rest>(entityID)...);
    }

    template <typename T>
    void ECS::RemoveComponent(int entityID) {
        GetComponentManager<T>()->RemoveComponent(entityID);
        refreshSystems_ = true;

        DistributeECSEvent(entityID, ECSAction::COMPONENT_REMOVE);
    }


    // By entity name.
    template <typename T, typename ...Args>
    ComponentWrapper<T> ECS::AddComponent(const std::string& entityName, const Args&... args) {
        return AddComponent<T, Args...>(GetNamedEntityID(entityName), args...);
    }

    template <typename T>
    bool ECS::HasComponent(const std::string& entityName) const {
        return HasComponent<T>(GetNamedEntityID(entityName));
    }

    template<typename... T>
    bool ECS::HasComponents(const std::string& entityName) const {
        return HasComponents<T...>(GetNamedEntityID(entityName));
    }

    template <typename T>
    ComponentWrapper<T> ECS::GetComponent(const std::string& entityName) const {
        return GetComponent<T>(GetNamedEntityID(entityName));
    }

    template <typename T1, typename T2, typename ...Rest>
    ComponentList ECS::GetComponents(const std::string& entityName) const {
        return GetComponents<T1, T2, Rest...>(GetNamedEntityID(entityName));
    }

    template <typename T>
    void ECS::RemoveComponent(const std::string& entityName) {
        RemoveComponent<T>(GetNamedEntityID(entityName));
    }

    template <typename T>
    void ECS::RegisterSystem(T* system) {
        static_assert(std::is_base_of_v<T, IComponentSystem>, "Template type T provided to RegisterSystem must derive from IComponentSystem.");

        std::type_index type = std::type_index(typeid(T));
        auto iterator = systems_.find(type);
        if (iterator == systems_.end()) {
            systems_.template emplace(type, system);
        }
        else {
            // TODO: This should be an assert.
            ImGuiLog::Instance().LogWarning("Attempting to register a system that has already been registered. Old system remains.");
        }
    }

    template <typename ...T, typename Fn>
    void ECS::IterateOver(Fn&& callback) {
        EntityComponentIterator<T...>* iterator = GetIterator<T...>();

        for (int entityID : iterator->GetValidEntityList()) {
            callback(*GetComponent<T>(entityID)...);
        }
    }

    template <typename T>
    ComponentManager<T>* ECS::AddComponentManager() {
        static_assert(std::is_base_of_v<IComponent, T>, "Template type T provided to AddComponentManager must derive from IComponent.");

        std::type_index componentTypeID = std::type_index(typeid(T));

        // Get associated component manager.
        ComponentManager<T>* componentManager;
        if (HasComponentManager<T>()) {
            componentManager = dynamic_cast<ComponentManager<T>*>(componentManagers_.at(componentTypeID));
        }
        else {
            // Type has not been registered, register new component manager.
            componentManager = new ComponentManager<T>();
            componentManagers_.template emplace(componentTypeID, componentManager);
        }

        assert(componentManager);
        return componentManager;
    }

    template <typename T>
    ComponentManager<T>* ECS::GetComponentManager() const {
        static_assert(std::is_base_of_v<IComponent, T>, "Template type T provided to GetComponentManager must derive from IComponent.");

        std::type_index componentTypeID = std::type_index(typeid(T));
        auto iterator = componentManagers_.find(componentTypeID);

        if (iterator != componentManagers_.end()) {
            ComponentManager<T>* componentManager = dynamic_cast<ComponentManager<T>*>(iterator->second);
            assert(componentManager);
            return componentManager;
        }
        else {
            return nullptr;
        }
    }

    template <typename T>
    bool ECS::HasComponentManager() const {
        static_assert(std::is_base_of_v<IComponent, T>, "Template type T provided to GetComponentManager must derive from IComponent.");

        std::type_index componentTypeID = std::type_index(typeid(T));
        auto iterator = componentManagers_.find(componentTypeID);

        if (iterator != componentManagers_.end()) {
            assert(dynamic_cast<ComponentManager<T>*>(iterator->second)); // Assert correct component manager mapping.
            return true;
        }
        else {
            return false;
        }
    }

    template<typename... T>
    EntityComponentIterator<T...>* ECS::GetIterator() {
        std::set<int> key { GetComponentID<T>()... };
        auto iterator = iteratorMapping_.find(key);

        EntityComponentIterator<T...>* iter;

        if (iterator == iteratorMapping_.end()) {
            // Register new Iterator.
            iter = new EntityComponentIterator<T...>();
            iteratorMapping_.emplace(key, iter);
        }
        else {
            iter = dynamic_cast<EntityComponentIterator<T...>*>(iterator->second);
        }

        if (!iter->Initialized()) {
            // Get the current (updated) state of the ECS system.
            std::vector<std::pair<int, ComponentList>> state;
            for (int entityID : entityManager_.GetEntityList()) {
                state.template emplace_back(entityID, GetComponents(entityID));
            }

            iter->Init(state);
        }

        return iter;
    }

    template<typename T>
    int ECS::GetComponentID() {
        std::type_index type = std::type_index(typeid(T));
        auto iterator = componentIDs_.find(type);

        int componentID = -1;

        if (iterator == componentIDs_.end()) {
            // Register new component ID.
            componentID = componentIDs_.size();
            componentIDs_.template emplace(type, componentID);
        }
        else {
            componentID = iterator->second;
        }

        return componentID;
    }

}
