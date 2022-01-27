
#ifndef SANDBOX_COMPONENT_MANAGER_H
#define SANDBOX_COMPONENT_MANAGER_H

#include "pch.h"

namespace Sandbox {

    class IComponentManager {
        public:
            virtual ~IComponentManager();

            virtual void Reset() = 0;
            virtual void RemoveComponent(int entityID) = 0;
    };

    template <typename T>
    class ComponentManager : public IComponentManager {
        public:
            ComponentManager();
            ~ComponentManager() override;

            void Reset() override; // Clears all components.

            // Constructs a component and returns it.
            template <typename ...Args>
            T* AddComponent(int entityID, const Args&... args);

            // Constructs a component and calls the callback function with it.
            template <typename Fn, typename ...Args>
            void SetComponent(int entityID, const Args& ...args, Fn&& callback);

            [[nodiscard]] T* GetComponent(int entityID) const;

            [[nodiscard]] bool HasComponent(int entityID) const;

            void RemoveComponent(int entityID) override;

        private:
            // Mappings.
            std::unordered_map<int, int> IDToIndex_;
            std::unordered_map<int, int> indexToID_;

            std::vector<T*> components_;
    };

}

#include "common/ecs/component/component_manager.tpp"

#endif //SANDBOX_COMPONENT_MANAGER_H
