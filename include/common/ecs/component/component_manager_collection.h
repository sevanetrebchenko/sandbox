
#ifndef SANDBOX_COMPONENT_MANAGER_COLLECTION_H
#define SANDBOX_COMPONENT_MANAGER_COLLECTION_H

#include "pch.h"
#include "common/ecs/component/component_manager.h"

// Component includes go here.
#include "common/geometry/transform.h"
#include "common/geometry/mesh.h"
#include "common/material/material.h"
#include "scenes/cs562/project1/light.h"

#define COMPONENT_TYPES Sandbox::Transform, Sandbox::Mesh, Sandbox::MaterialCollection, Sandbox::LocalLight

namespace Sandbox {

    template <typename ...T>
    class ComponentManagerCollection {
        public:
            ComponentManagerCollection();
            ~ComponentManagerCollection();

            void Init();
            void Reset();  // Clears data between scenes.
            void Shutdown();

            // Retrieves component manager for the given type.
            // Creates new component manager if queried one does not exist.
            template <typename Y>
            [[nodiscard]] ComponentManager<Y>* GetComponentManager() const;

            // Removes all components from an entity.
            void RemoveAllComponents(int entityID);

        private:
            template <typename Y>
            void CreateComponentManager();

            template <typename Y>
            void ResetComponentManager();

            template <typename Y>
            void RemoveComponent(int entityID);

            template <typename Y>
            void GenerateTypeID();

            template <typename Y>
            [[nodiscard]] int GetIDFromType() const;

            std::unordered_map<int, IComponentManager*> componentManagers_;

            int typeIDCounter_;
            std::unordered_map<std::type_index, int> typeIDMapping_;
    };

}

#include "common/ecs/component/component_manager_collection.tpp"

#endif //SANDBOX_COMPONENT_MANAGER_COLLECTION_H
