
#ifndef SANDBOX_ECS_TPP
#define SANDBOX_ECS_TPP

namespace Sandbox {

    template <typename ...T, typename Fn>
    void ECS::IterateOver(Fn&& callback) const {
        static std::vector<int> validEntities;

        // Update valid entities list.
        // TODO: Change to only iterate over the changed entities of the last frame.
        for (int entityID : entityManager_.GetEntityList()) {
            // HasComponents... is safe to call for entities that don't exist (returns false).
            bool hasRequiredComponents = HasComponents<T...>(entityID);

            // Determine whether entity is present in valid entities list.
            bool existsInEntityList = false;
            int validEntityIndex = -1;
            int numValidEntities = validEntities.size();

            for (int i = 0; i < numValidEntities; ++i) {
                if (validEntities[i] == entityID) {
                    existsInEntityList = true;
                    validEntityIndex = i;
                    break;
                }
            }

            if (existsInEntityList) {
                if (!hasRequiredComponents) {
                    // Exists in the list, but doesn't have the required components.
                    // Remove from the list.
                    std::swap(validEntities[validEntityIndex], validEntities[numValidEntities - 1]);
                    validEntities.pop_back();
                }
            }
            else {
                if (hasRequiredComponents) {
                    // Doesn't exist in the list, and has the required components.
                    // Add to the list.
                    validEntities.template emplace_back(entityID);
                }
            }
        }

        // Get the queried components from each of the valid entities.
        for (int entityID : validEntities) {
            callback(*GetComponent<T>(entityID)...);
        }
    }

    template <typename T, typename ...Args>
    T* ECS::AddComponent(int entityID, const Args&... args) {
        T* component;

        if (componentManagers_.template HasComponentManager<T>()) {
            component = componentManagers_.template GetComponentManager<T>()->template AddComponent(entityID, args...);
        }
        else {
            // ComponentManager for requested type does not exist, and must be created first.
            component = componentManagers_.template AddComponentManager<T>()->template AddComponent(entityID, args...);
        }

        refreshSystems_ = true;
        changedEntities_.template emplace(entityID);

        return component;
    }

    template <typename T, typename ...Args>
    T* ECS::AddComponent(const std::string& entityName, const Args&... args) {
        return AddComponent<T, Args...>(GetNamedEntityID(entityName), args...);
    }

    template <typename T, typename Fn, typename ...Args>
    void ECS::SetComponent(int entityID, const Args&... args, Fn&& callback) {
        if (componentManagers_.template HasComponentManager<T>()) {
            componentManagers_.template GetComponentManager<T>()->template SetComponent(entityID, args..., std::forward<Fn>(callback));
        }
        else {
            // ComponentManager for requested type does not exist, and must be created first.
            componentManagers_.template AddComponentManager<T>()->template SetComponent(entityID, args..., std::forward<Fn>(callback));
        }

        refreshSystems_ = true;
        changedEntities_.template emplace(entityID);
    }

    template <typename T, typename Fn, typename ...Args>
    void ECS::SetComponent(const std::string& entityName, const Args&... args, Fn&& callback) {
        SetComponent<T>(GetNamedEntityID(entityName), args..., std::forward<Fn>(callback));
    }

    template <typename T>
    bool ECS::HasComponent(int entityID) const {
        // Short circuit.
        return componentManagers_.template HasComponentManager<T>() &&
               componentManagers_.template GetComponentManager<T>()->HasComponent(entityID);
    }

    template <typename T>
    bool ECS::HasComponent(const std::string& entityName) const {
        return HasComponent<T>(GetNamedEntityID(entityName));
    }

    template<typename... T>
    bool ECS::HasComponents(int entityID) const {
        return (HasComponent<T>(entityID) && ...);
    }

    template<typename... T>
    bool ECS::HasComponents(const std::string& entityName) const {
        return HasComponents<T...>(GetNamedEntityID(entityName));
    }

    template <typename T>
    T* ECS::GetComponent(int entityID) const {
        if (componentManagers_.template HasComponentManager<T>()) {
            return componentManagers_.template GetComponentManager<T>()->GetComponent(entityID);
        }
        else {
            // No registered component manager, means no entities have that component type.
            return nullptr;
        }
    }

    template <typename T>
    T* ECS::GetComponent(const std::string& entityName) const {
        return GetComponent<T>(GetNamedEntityID(entityName));
    }

    template <typename T>
    void ECS::RemoveComponent(int entityID) {
        componentManagers_.template GetComponentManager<T>()->RemoveComponent(entityID);

        refreshSystems_ = true;
        changedEntities_.template emplace(entityID);
    }

    template <typename T>
    void ECS::RemoveComponent(const std::string& entityName) {
        RemoveComponent<T>(GetNamedEntityID(entityName));
    }

}

#endif //SANDBOX_ECS_TPP