
#include <utility>

#include "common/ecs/ecs.h"
#include "common/geometry/transform.h"

namespace Sandbox {

    ECS& ECS::Instance() {
        static ECS instance;
        return instance;
    }

    ECS::ECS() : refreshSystems_(false) {
    }

    ECS::~ECS() {
    }

    void ECS::Init() {
        for (std::pair<const std::type_index, IComponentSystem*>& systemData : systems_) {
            IComponentSystem* system = systemData.second;
            system->Init();
        }
    }

    void ECS::Update() {
        // Ensure systems are processing the latest (most up-to-date) list of entities.
        if (refreshSystems_) {
            std::vector<int> entityList = entityManager_.GetEntityList();

            for (std::pair<const std::type_index, IComponentSystem*>& systemData : systems_) {
                IComponentSystem* system = systemData.second;
                for (int entityID : entityList) {
                    bool managed = system->ManagesEntity(entityID);

                    if (system->CheckEntityComponents(entityID)) {
                        if (!managed) {
                            system->AddEntity(entityID);
                        }
                    }
                    else {
                        if (managed) {
                            system->RemoveEntity(entityID);
                        }
                    }
                }
            }

            refreshSystems_ = false;
        }

        for (std::pair<const std::type_index, IComponentSystem*>& systemData : systems_) {
            IComponentSystem* system = systemData.second;
            system->Update();
        }
    }

    void ECS::Reset() {
        // Reset entity manager.
        entityManager_.Reset();

        // Reset component managers.
        for (std::pair<const std::type_index, IComponentManager*>& componentManagerData : componentManagers_) {
            IComponentManager* componentManager = componentManagerData.second;
            componentManager->Reset();
        }

        // Reset systems.
        for (std::pair<const std::type_index, IComponentSystem*>& systemData : systems_) {
            IComponentSystem* componentSystem = systemData.second;
            // componentSystem->Reset();
        }

        // Reset iterators.
        for (std::pair<const std::set<int>, IEntityComponentIterator*>& iteratorData : iteratorMapping_) {
            IEntityComponentIterator* iterator = iteratorData.second;
            iterator->Reset();
        }
    }

    void ECS::Shutdown() {
        for (std::pair<const std::type_index, IComponentSystem*>& systemData : systems_) {
            IComponentSystem* system = systemData.second;
            system->Shutdown();
        }
    }

    int ECS::CreateEntity(const std::string& entityName) {
        int entityID = entityManager_.CreateEntity(entityName);

        // All entities have a transform component.
        AddComponent<Transform>(entityID);

        refreshSystems_ = true;
        DistributeECSEvent(entityID, ECSAction::ENTITY_CREATE);

        return entityID;
    }

    void ECS::DestroyEntity(int entityID) {
        // Remove entity components from all component managers.
        for (std::pair<const std::type_index, IComponentManager*>& componentManagerData : componentManagers_) {
            IComponentManager* componentManager = componentManagerData.second;
            componentManager->RemoveComponent(entityID);
        }

        // Remove entity.
        entityManager_.DestroyEntity(entityID);

        refreshSystems_ = true;
        DistributeECSEvent(entityID, ECSAction::ENTITY_DESTROY);
    }

    void ECS::DestroyEntity(const std::string& entityName) {
        DestroyEntity(GetNamedEntityID(entityName));
    }

    int ECS::GetNamedEntityID(const std::string& entityName) const {
        return entityManager_.GetNamedEntityID(entityName);
    }

    ComponentList ECS::GetComponents(int entityID) const {
        std::unordered_map<std::type_index, IComponent*> mappingList;

        for (const std::pair<const std::type_index, IComponentManager*>& componentManagerData : componentManagers_) {
            std::type_index type = componentManagerData.first;
            IComponentManager* componentManager = componentManagerData.second;

            if (componentManager->HasComponent(entityID)) {
                mappingList.emplace(type, componentManager->GetComponent(entityID));
            }
        }

        return ComponentList { mappingList };
    }

    ComponentList ECS::GetComponents(const std::string& entityName) const {
        return GetComponents(GetNamedEntityID(entityName));
    }

    void ECS::DistributeECSEvent(int entityID, ECSAction::Type actionType) {
        ECSAction action { actionType, entityID, GetComponents(entityID) };

        for (const std::pair<const std::set<int>, IEntityComponentIterator*>& data : iteratorMapping_) {
            IEntityComponentIterator* iterator = data.second;
            iterator->ApplyAction(action);
        }
    }

}
