
#pragma once

namespace Sandbox {

    template<typename... T>
    EntityComponentIterator<T...>::EntityComponentIterator() : IEntityComponentIterator() {
    }

    template<typename... T>
    EntityComponentIterator<T...>::~EntityComponentIterator() {
    }

    template <typename ...T>
    bool EntityComponentIterator<T...>::ValidateEntityComponentIDs(const ComponentList& componentList) const {
        return (componentList.HasComponent<T>() && ...);
    }

    template<typename... T>
    void EntityComponentIterator<T...>::Init(const std::vector<std::pair<int, ComponentList>>& state) {
        // Apply ECS system state.
        for (const std::pair<int, ComponentList>& entityState : state) {
            int entityID = entityState.first;
            const ComponentList& componentList = entityState.second;

            if (ValidateEntityComponentIDs(componentList)) {
                validEntityList_.emplace(entityID);
            }
        }

        initialized_ = true;
    }

}