
#include "common/ecs/entity/entity_manager.h"

namespace Sandbox {

    EntityManager::EntityManager() : entityCounter_(0) {
    }

    EntityManager::~EntityManager() {
    }

    int EntityManager::CreateEntity(std::string entityName) {
        int entityID = -1; // Invalid ID.

        if (!recycledEntityIDs_.empty()) {
            // Use recycled entity ID to minimize gaps in vector.
            entityID = recycledEntityIDs_.top();
            recycledEntityIDs_.pop();
        }
        else {
            entityID = entityCounter_++;
        }

        if (entityName.empty()) {
            entityName = "New Entity";
        }

        // Ensure unique entity name.
        auto entityNameIter = nameToID_.find(entityName);
        if (entityNameIter != nameToID_.end()) {
            entityName = GetUniqueEntityName(entityName);
        }
        assert(nameToID_.find(entityName) == nameToID_.end());

        // Register mappings.
        nameToID_.emplace(entityName, entityID);
        IDToName_.emplace(entityID, entityName);

        return entityID;
    }

    // The assumption made here is that, since the EntityManager controls how entities are created, the only error
    // checking that needs to be done is to check for the existence of an entity with the given ID.
    void EntityManager::DestroyEntity(int entityID) {
        auto idIterator = IDToName_.find(entityID);
        if (idIterator == IDToName_.end()) {
            return; // Entity does not exist.
        }
        const std::string& entityName = idIterator->second;

        // Erase mappings.
        IDToName_.erase(entityID);
        nameToID_.erase(entityName);

        // Register new valid entity ID.
        recycledEntityIDs_.push(entityID);
    }

    void EntityManager::DestroyEntity(const std::string& entityName) {
        auto iterator = nameToID_.find(entityName);
        if (iterator != nameToID_.end()) {
            DestroyEntity(iterator->second);
        }
    }

    int EntityManager::GetNamedEntityID(const std::string& entityName) const {
        auto iterator = nameToID_.find(entityName);
        if (iterator != nameToID_.end()) {
            return iterator->second;
        }
        else {
            return -1;
        }
    }

    std::string EntityManager::GetUniqueEntityName(const std::string& entityName) const {
        int append = 1;
        std::string appended = entityName + std::to_string(append);

        while (nameToID_.find(appended) != nameToID_.end()) {
            ++append;
            appended = entityName + std::to_string(append);
        }

        return appended;
    }

}
