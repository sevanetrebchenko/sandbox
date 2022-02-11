
#pragma once

#include "pch.h"
#include "common/ecs/entity/entity_manager.h"
#include "common/ecs/component/component_manager.h"
#include "common/ecs/system/component_system.h"
#include "common/ecs/component/component_list.h"
#include "common/ecs/iterator/entity_component_iterator.h"
#include "common/ecs/component/component_wrapper.h"

namespace Sandbox {

    // Public interface for everything to do with managing entities and their components.
    class ECS {
        public:
            static ECS& Instance();

            void Init();
            void Update(); // Updates all systems.
            void Reset();  // Clears data between scenes.
            void Shutdown();


            // Entity management.
            [[nodiscard]] int CreateEntity(const std::string& entityName = "");

            void DestroyEntity(int entityID);
            void DestroyEntity(const std::string& entityName);

            [[nodiscard]] int GetNamedEntityID(const std::string& entityName) const;


            // Component management.
            // Types should match a built-in component type, without any decorations (const, pointer, reference, volatile, etc).

            // By entity ID.
            // Throws error if component at given entity ID already exists.
            template <typename T, typename ...Args>
            ComponentWrapper<T> AddComponent(int entityID, const Args&... args);

            template <typename T>
            [[nodiscard]] bool HasComponent(int entityID) const;

            template <typename ...T>
            [[nodiscard]] bool HasComponents(int entityID) const;

            template <typename T>
            [[nodiscard]] ComponentWrapper<T> GetComponent(int entityID) const;

            // Gets the requested components currently attached to the entity.
            // Query requires at least two component types.
            template <typename T1, typename T2, typename ...Rest>
            [[nodiscard]] ComponentList GetComponents(int entityID) const;

            // Gets all components currently attached to the entity.
            [[nodiscard]] ComponentList GetComponents(int entityID) const;

            template <typename T>
            void RemoveComponent(int entityID);


            // By entity name.
            // Throws error if component at given entity name already exists.
            template <typename T, typename ...Args>
            ComponentWrapper<T> AddComponent(const std::string& entityName, const Args&... args);

            template <typename T>
            [[nodiscard]] bool HasComponent(const std::string& entityName) const;

            template <typename ...T>
            [[nodiscard]] bool HasComponents(const std::string& entityName) const;

            template <typename T>
            [[nodiscard]] ComponentWrapper<T> GetComponent(const std::string& entityName) const;

            // Gets the requested components currently attached to the entity.
            // Query requires at least two component types.
            template <typename T1, typename T2, typename ...Rest>
            [[nodiscard]] ComponentList GetComponents(const std::string& entityName) const;

            // Gets all components currently attached to the entity.
            [[nodiscard]] ComponentList GetComponents(const std::string& entityName) const;

            template <typename T>
            void RemoveComponent(const std::string& entityName);


            // System management.
            template <typename T>
            void RegisterSystem(T* system);

            // Calls the callback function for each entity, given it has the required set of components.
            template <typename ...T, typename Fn>
            inline void IterateOver(Fn&& callback);


        private:
            ECS();
            ~ECS();

            template <typename T>
            ComponentManager<T>* AddComponentManager();

            // Retrieves component manager for the given type, given that it exists.
            // Otherwise, returns nullptr.
            template <typename T>
            [[nodiscard]] ComponentManager<T>* GetComponentManager() const;

            template <typename T>
            [[nodiscard]] bool HasComponentManager() const;

            template <typename ...T>
            [[nodiscard]] EntityComponentIterator<T...>* GetIterator();

            void DistributeECSEvent(int entityID, ECSAction::Type actionType);

            template <typename T>
            [[nodiscard]] int GetComponentID();

            // Entity management.
            EntityManager entityManager_;

            // Component management.
            std::unordered_map<std::type_index, IComponentManager*> componentManagers_;

            // System management.
            std::unordered_map<std::type_index, IComponentSystem*> systems_;
            bool refreshSystems_;

            std::unordered_map<std::type_index, int> componentIDs_;
            std::map<std::set<int>, IEntityComponentIterator*> iteratorMapping_;
    };

}

#include "common/ecs/ecs.tpp"
