
#include "common/ecs/component/component_list.h"

namespace Sandbox {

    ComponentList::ComponentList(const std::unordered_map<std::type_index, IComponent*>& componentPairing) {
        for (const std::pair<const std::type_index, IComponent*>& mapping : componentPairing) {
            AddComponent(mapping.first, mapping.second);
        }
    }

    ComponentList::~ComponentList() {
    }

    void ComponentList::AddComponent(std::type_index type, IComponent* component) {
        auto iterator = componentMapping_.find(type);

        if (iterator == componentMapping_.end()) {
            // Component does not yet exist, register.
            componentMapping_[type] = component;
        }
        else {
            // Attempting to override already existing component is an error.
            throw std::runtime_error("From ComponentList::AddComponent: attempting to override an already existing component.");
        }
    }

}
