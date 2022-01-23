
#ifndef SANDBOX_ECS_TPP
#define SANDBOX_ECS_TPP

namespace Sandbox {

    template <typename T, typename ...Args>
    T* ECS::AddComponent(int entityID, Args... args) {
        ComponentManager<T>* componentManager = componentManagers_.template GetComponentManager<T>();
        T* component =  componentManager->AddComponent(entityID, std::forward<Args>(args)...);

        refreshSystems_ = true;

        return component;
    }

    template <typename T, typename ...Args>
    T* ECS::AddComponent(const std::string& entityName, Args... args) {
        return AddComponent<T, Args...>(GetNamedEntityID(entityName), std::forward<Args>(args)...);
    }

    template <typename T>
    bool ECS::HasComponent(int entityID) const {
        ComponentManager<T>* componentManager = componentManagers_.template GetComponentManager<T>();
        return componentManager->HasComponent(entityID);
    }

    template <typename T>
    bool ECS::HasComponent(const std::string& entityName) const {
        return HasComponent<T>(GetNamedEntityID(entityName));
    }

    template <typename T>
    T* ECS::GetComponent(int entityID) {
        ComponentManager<T>* componentManager = componentManagers_.template GetComponentManager<T>();
        return componentManager->GetComponent(entityID);
    }

    template <typename T>
    T* ECS::GetComponent(const std::string& entityName) {
        return GetComponent<T>(GetNamedEntityID(entityName));
    }

    template <typename T>
    void ECS::RemoveComponent(int entityID) {
        ComponentManager<T>* componentManager = componentManagers_.template GetComponentManager<T>();
        componentManager->RemoveComponent(entityID);

        refreshSystems_ = true;
    }

    template <typename T>
    void ECS::RemoveComponent(const std::string& entityName) {
        RemoveComponent<T>(GetNamedEntityID(entityName));
    }

}

#endif //SANDBOX_ECS_TPP