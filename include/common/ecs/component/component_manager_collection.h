
#ifndef SANDBOX_COMPONENT_MANAGER_COLLECTION_H
#define SANDBOX_COMPONENT_MANAGER_COLLECTION_H

#include "pch.h"
#include "common/ecs/component/component_manager.h"

namespace Sandbox {

    class ComponentManagerCollection {
        public:
            ComponentManagerCollection();
            ~ComponentManagerCollection();

            void Init();
            void Reset();  // Clears data between scenes.
            void Shutdown();

            template <typename T>
            ComponentManager<T>* AddComponentManager();

            // Retrieves component manager for the given type, given that it exists.
            // Otherwise, returns nullptr.
            template <typename T>
            [[nodiscard]] ComponentManager<T>* GetComponentManager() const;

            template <typename T>
            [[nodiscard]] bool HasComponentManager() const;

            // Removes all components from an entity.
            void RemoveAllComponents(int entityID);

        private:
            std::vector<IComponentManager*> componentManagers_;
            std::unordered_map<std::type_index, int> typeIDMapping_;
    };

}

#include "common/ecs/component/component_manager_collection.tpp"

#endif //SANDBOX_COMPONENT_MANAGER_COLLECTION_H
