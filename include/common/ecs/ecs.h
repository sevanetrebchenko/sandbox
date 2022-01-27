
#ifndef SANDBOX_ECS_H
#define SANDBOX_ECS_H

#include "common/ecs/entity/entity_manager.h"
#include "common/ecs/component/component_manager_collection.h"
#include "common/ecs/system/component_system.h"

namespace Sandbox {

    // Public interface for everything to do with managing entities and their components.
    class ECS {
        public:
            static ECS& Instance();

            void Init();
            void Update(); // Updates all systems.
            void Reset();  // Clears data between scenes.
            void Shutdown();

            void RegisterSystem(ISystem* system);

            // Calls the given callback function for each entity, given it has the required set of components.
            template <typename ...T, typename Fn>
            void IterateOver(Fn&& callback) const;


            // Entity management.
            [[nodiscard]] int CreateEntity(const std::string& entityName = "");

            void DestroyEntity(int entityID);
            void DestroyEntity(const std::string& entityName);

            [[nodiscard]] int GetNamedEntityID(const std::string& entityName) const;


            // Component management.
            // Types should match a built-in component type, without any decorations (const, pointer, reference, volatile, etc).
            template <typename T, typename ...Args>
            T* AddComponent(int entityID, const Args&... args);

            template <typename T, typename Fn, typename ...Args>
            void SetComponent(int entityID, const Args&... args, Fn&& callback);

            template <typename T>
            [[nodiscard]] bool HasComponent(int entityID) const;

            template <typename ...T>
            [[nodiscard]] bool HasComponents(int entityID) const;

            template <typename T>
            [[nodiscard]] T* GetComponent(int entityID) const;

            template <typename T>
            void RemoveComponent(int entityID);


            template <typename T, typename ...Args>
            T* AddComponent(const std::string& entityName, const Args&... args);

            template <typename T, typename Fn, typename ...Args>
            void SetComponent(const std::string& entityName, const Args&... args, Fn&& callback);

            template <typename T>
            [[nodiscard]] bool HasComponent(const std::string& entityName) const;

            template <typename ...T>
            [[nodiscard]] bool HasComponents(const std::string& entityName) const;

            template <typename T>
            [[nodiscard]] T* GetComponent(const std::string& entityName) const;

            template <typename T>
            void RemoveComponent(const std::string& entityName);

        private:
            ECS();
            ~ECS();

            EntityManager entityManager_;
            ComponentManagerCollection<COMPONENT_TYPES> componentManagers_;
            std::vector<ISystem*> systems_;

            bool refreshSystems_;
            std::set<int> changedEntities_; // Entities that were updated this frame (created, destroyed, add/remove component).
    };

}

#include "common/ecs/ecs.tpp"

#endif //SANDBOX_ECS_H
