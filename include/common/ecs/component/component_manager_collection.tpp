
#ifndef SANDBOX_COMPONENT_MANAGER_COLLECTION_TPP
#define SANDBOX_COMPONENT_MANAGER_COLLECTION_TPP

#include "common/ecs/component/component.h"

namespace Sandbox {

    template <typename T>
    ComponentManager<T>* ComponentManagerCollection::GetComponentManager() {
        static_assert(std::is_base_of_v<IComponent, T>, "Template type T provided to GetComponentManager must derive from IComponent.");

        int type = GetTypeID<T>();
        auto iterator = componentManagers_.find(type);

        if (iterator == componentManagers_.end()) {
            // Component manager does not exist, needs to be created.
            componentManagers_.template emplace(type, new ComponentManager<T>());
        }

        ComponentManager<T>* componentManager = dynamic_cast<ComponentManager<T>*>(componentManagers_[type]);
        assert(componentManager); // Should always succeed.

        return componentManager;
    }

    template <typename T>
    int ComponentManagerCollection::GetTypeID() {
        // Static asserts ensure type exists.
        std::type_index type = std::type_index(typeid(T));
        auto iterator = typeIDMapping_.find(type);

        if (iterator == typeIDMapping_.end()) {
            // Type has not been registered yet.
            typeIDMapping_.template emplace(type, typeIDCounter_++);
        }

        return typeIDMapping_[type];
    }

}

#endif //SANDBOX_COMPONENT_MANAGER_COLLECTION_TPP
