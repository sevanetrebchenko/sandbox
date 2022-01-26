
#include "common/ecs/system/component_system.h"

namespace Sandbox {

    void ISystem::Init() {
    }

    void ISystem::Shutdown() {
    }

    bool ISystem::ManagesEntity(int entityID) const {
        for (int entity : entityIDs_) {
            if (entityID == entity) {
                return true;
            }
        }

        return false;
    }

    void ISystem::AddEntity(int entityID) {
        if (!ManagesEntity(entityID)) {
            entityIDs_.emplace_back(entityID);
        }
    }

    void ISystem::RemoveEntity(int entityID) {
        int numEntities = entityIDs_.size();

        for (int i = 0; i < numEntities; ++i) {
            if (entityIDs_[i] == entityID) {
                // Swap paradigm.
                std::swap(entityIDs_[i], entityIDs_[numEntities - 1]);
                entityIDs_.pop_back();
                break;
            }
        }
    }
}