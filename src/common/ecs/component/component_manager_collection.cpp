
#include "common/ecs/component/component_manager_collection.h"

namespace Sandbox {

    ComponentManagerCollection::ComponentManagerCollection() : typeIDCounter_(0) {
    }

    ComponentManagerCollection::~ComponentManagerCollection() {
    }

    void ComponentManagerCollection::Init() {
    }

    void ComponentManagerCollection::Reset() {
        // Clear all components from registered ComponentManagers.
        for (const std::pair<const int, IComponentManager*>& data : componentManagers_) {
            IComponentManager* componentManager = data.second;
            componentManager->Reset();
        }
    }

    void ComponentManagerCollection::Shutdown() {
        Reset();

        // Delete all component managers.
        for (const std::pair<const int, IComponentManager*>& data : componentManagers_) {
            delete data.second;
        }
    }

    void ComponentManagerCollection::RemoveAllComponents(int entityID) {
        for (const std::pair<const int, IComponentManager*>& data : componentManagers_) {
            IComponentManager* componentManager = data.second;
            componentManager->RemoveComponent(entityID);
        }
    }

}
