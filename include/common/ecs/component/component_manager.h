
#ifndef SANDBOX_COMPONENT_MANAGER_H
#define SANDBOX_COMPONENT_MANAGER_H

#include "pch.h"

namespace Sandbox {

    class IComponentManager {
        public:
            virtual ~IComponentManager() = 0;
    };

    template <typename T>
    class ComponentManager : public IComponentManager {
        public:
            ComponentManager();
            ~ComponentManager() override;

            void Reset(); // Clears all components.

            // Pointers to components should not be stored over the course of multiple frames.

            // Returns new component, or
            template <typename ...Args>
            T* AddComponent(int entityID, Args... args);

            [[nodiscard]] T* GetComponent(int entityID) const;
            [[nodiscard]] bool HasComponent(int entityID) const;
            void RemoveComponent(int entityID);

        private:
            // Mappings.
            std::unordered_map<int, int> IDToIndex_;
            std::unordered_map<int, int> indexToID_;

            std::vector<T> components_;
    };

}

#include "common/ecs/component/component_manager.tpp"

#endif //SANDBOX_COMPONENT_MANAGER_H
