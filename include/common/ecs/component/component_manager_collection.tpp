
#ifndef SANDBOX_COMPONENT_MANAGER_COLLECTION_TPP
#define SANDBOX_COMPONENT_MANAGER_COLLECTION_TPP

#include "common/ecs/component/component.h"

namespace Sandbox {

    template <typename T>
    ComponentManager<T>* ComponentManagerCollection::AddComponentManager() {
        static_assert(std::is_base_of_v<IComponent, T>, "Template type T provided to AddComponentManager must derive from IComponent.");

        std::type_index type = std::type_index(typeid(T));
        ComponentManager<T>* componentManager;

        if (HasComponentManager<T>()) {
            componentManager = dynamic_cast<ComponentManager<T>*>(componentManagers_[typeIDMapping_.at(type)]);
        }
        else {
            // Type has not been registered, register new component manager.
            componentManager = new ComponentManager<T>();

            typeIDMapping_.template emplace(type, componentManagers_.size());
            componentManagers_.template emplace_back(componentManager);
        }

        assert(componentManager);
        return componentManager;
    }

    template <typename T>
    ComponentManager<T>* ComponentManagerCollection::GetComponentManager() const {
        static_assert(std::is_base_of_v<IComponent, T>, "Template type T provided to GetComponentManager must derive from IComponent.");

        if (HasComponentManager<T>()) {
            std::type_index type = std::type_index(typeid(T));
            int typeID = typeIDMapping_.at(type);

            ComponentManager<T>* componentManager = dynamic_cast<ComponentManager<T>*>(componentManagers_[typeID]);
            assert(componentManager); // Should always succeed.

            return componentManager;
        }
        else {
            return nullptr;
        }
    }

    template <typename T>
    bool ComponentManagerCollection::HasComponentManager() const {
        static_assert(std::is_base_of_v<IComponent, T>, "Template type T provided to GetComponentManager must derive from IComponent.");

        // Attempt to get registered type mapping.
        std::type_index type = std::type_index(typeid(T));
        auto iterator = typeIDMapping_.find(type);

        if (iterator == typeIDMapping_.end()) {
            // Type has not been registered, hence component manager does not exist.
            return false;
        }
        else {
            int typeID = iterator->second;
            assert(typeID < componentManagers_.size());
            assert(dynamic_cast<ComponentManager<T>*>(componentManagers_[typeID])); // Assert correct mapping.

            return true;
        }
    }

}

#endif //SANDBOX_COMPONENT_MANAGER_COLLECTION_TPP
