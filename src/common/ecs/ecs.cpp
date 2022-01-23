
#include "common/ecs/ecs.h"
#include "common/ecs/component/types/transform.h"

namespace Sandbox {

    ECS& ECS::Instance() {
        static ECS instance;
        return instance;
    }

    ECS::ECS() : refreshSystems_(false) {
    }

    ECS::~ECS() {
    }

    void ECS::RegisterSystem(ISystem* system) {
        // TODO: Checks to ensure no system duplication?
        systems_.emplace_back(system);
    }

    void ECS::Init() {
        for (ISystem* system : systems_) {
            system->Init();
        }
    }

    void ECS::Update() {
        // Ensure systems are processing the latest (most up-to-date) list of entities.
        if (refreshSystems_) {
            std::vector<int> entityList = entityManager_.GetEntityList();

            for (ISystem* system : systems_) {
                for (int entityID : entityList) {
                    system->CheckEntityComponents(entityID);
                }
            }

            refreshSystems_ = false;
        }

        for (ISystem* system : systems_) {
            system->Update();
        }
    }

    void ECS::Reset() {
        entityManager_.Reset();
        componentManagers_.Reset();
    }

    void ECS::Shutdown() {
        for (ISystem* system : systems_) {
            system->Shutdown();
        }
    }

    int ECS::CreateEntity(const std::string& entityName) {
        int entityID = entityManager_.CreateEntity(entityName);

        // All entities have a transform component.
        AddComponent<Transform>(entityID);

        refreshSystems_ = true;

        return entityID;
    }

    void ECS::DestroyEntity(int entityID) {
        // Remove entity components.
        componentManagers_.RemoveAllComponents(entityID);

        // Remove entity.
        entityManager_.DestroyEntity(entityID);

        refreshSystems_ = true;
    }

    void ECS::DestroyEntity(const std::string& entityName) {
        DestroyEntity(GetNamedEntityID(entityName));
    }

    int ECS::GetNamedEntityID(const std::string& entityName) const {
        return entityManager_.GetNamedEntityID(entityName);
    }

}
