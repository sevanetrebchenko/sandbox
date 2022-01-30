
#pragma once

#include "pch.h"
#include "common/ecs/component/component.h"

namespace Sandbox {

    class IComponentManager {
        public:
            using ComponentType = void;

            virtual ~IComponentManager();

            virtual void Reset() = 0;

            [[nodiscard]] virtual IComponent* GetComponent(int entityID) const = 0;
            [[nodiscard]] virtual bool HasComponent(int entityID) const = 0;
            virtual void RemoveComponent(int entityID) = 0;
    };

    template <typename T>
    class ComponentManager : public IComponentManager {
        public:
            using ComponentType = T;

            ComponentManager();
            ~ComponentManager() override;

            void Reset() override; // Clears all components.

            // Constructs a component and returns it.
            // Throws if component at the given entity ID already exists.
            template <typename ...Args>
            T* AddComponent(int entityID, const Args&... args);

            // Constructs a component and calls the callback function with it.
            // If component at the given entity ID already exists, calls the callback function on the existing component.
            template <typename Fn, typename ...Args>
            void SetComponent(int entityID, const Args& ...args, Fn&& callback);

            [[nodiscard]] T* GetComponent(int entityID) const override;

            [[nodiscard]] bool HasComponent(int entityID) const override;

            void RemoveComponent(int entityID) override;

        private:
            // Mappings.
            std::unordered_map<int, int> IDToIndex_;
            std::unordered_map<int, int> indexToID_;

            std::vector<T*> components_;
    };

}

#include "common/ecs/component/component_manager.tpp"
