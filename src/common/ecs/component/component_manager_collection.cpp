
#include "common/ecs/component/component_manager_collection.h"

namespace Sandbox {

    ComponentManagerCollection::ComponentManagerCollection() {
    }

    ComponentManagerCollection::~ComponentManagerCollection() {
    }

    void ComponentManagerCollection::Init() {
    }

    void ComponentManagerCollection::Reset() {
        // Clear all components from registered ComponentManagers.
        for (IComponentManager* componentManager : componentManagers_) {
            componentManager->Reset();
        }
    }

    void ComponentManagerCollection::Shutdown() {
        Reset();

        // Delete all component managers.
        for (IComponentManager* componentManager : componentManagers_) {
            delete componentManager;
        }

        componentManagers_.clear();
    }

    void ComponentManagerCollection::RemoveAllComponents(int entityID) {
        for (IComponentManager* componentManager : componentManagers_) {
            componentManager->RemoveComponent(entityID);
        }
    }

}
