
#include "common/ecs/ecs.h"
#include "common/ecs/component/types/transform.h"

namespace Sandbox {

    ECS& ECS::Instance() {
        static ECS instance;
        return instance;
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
        for (ISystem* system : systems_) {
            system->Update();
        }
    }

    void ECS::Reset() {

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

        return entityID;
    }

    void ECS::DestroyEntity(int entityID) {
        // Remove entity components.

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

    ECS::ECS() {
    }

    ECS::~ECS() {
    }

}
