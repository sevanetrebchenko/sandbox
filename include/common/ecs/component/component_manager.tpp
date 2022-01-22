
#ifndef SANDBOX_COMPONENT_MANAGER_TPP
#define SANDBOX_COMPONENT_MANAGER_TPP

#include "component_manager.h"

namespace Sandbox {

    template <typename T>
    ComponentManager<T>::ComponentManager() {
    }

    template<typename T>
    ComponentManager<T>::~ComponentManager() {
    }

    template<typename T>
    void ComponentManager<T>::Reset() {
        components_.clear();

        IDToIndex_.clear();
        indexToID_.clear();
    }

    template<typename T>
    T* ComponentManager<T>::AddComponent(int entityID) {
        auto iterator = IDToIndex_.find(entityID);
        if (iterator != IDToIndex_.end()) {
            // Component already exists at this entity ID.
            return &components_[iterator->second];
        }

        int index = components_.size();
        components_.emplace_back(); // TODO: pass in args to allow non-default construction.

        // Register mapping.
        IDToIndex_.emplace(entityID, index);
        indexToID_.emplace(index, entityID);

        return &components_[index];
    }

    template<typename T>
    T* ComponentManager<T>::GetComponent(int entityID) const {
        auto iterator = IDToIndex_.find(entityID);
        if (iterator != IDToIndex_.end()) {
            return &components_[iterator->second];
        }
        else {
            // Component does not exist.
            return nullptr;
        }
    }

    template<typename T>
    bool ComponentManager<T>::HasComponent(int entityID) const {
        auto iterator = IDToIndex_.find(entityID);
        return iterator != IDToIndex_.end();
    }

    template<typename T>
    void ComponentManager<T>::RemoveComponent(int entityID) {
        auto indexIterator = IDToIndex_.find(entityID);
        if (indexIterator == IDToIndex_.end()) {
            // Entity does not have this component attached to it.
            return;
        }

        int entityComponentIndex = indexIterator->second;

        // Erase deleted component mapping.
        IDToIndex_.erase(entityID);
        indexToID_.erase(entityComponentIndex);


        int lastIndex = components_.size() - 1; // Guaranteed to be positive.
        int lastEntityID = indexToID_[lastIndex];

        std::swap(components_[entityComponentIndex], components_[lastIndex]);
        components_.pop_back(); // Delete component.

        // Update mapping.
        indexToID_[entityComponentIndex] = lastEntityID;
        IDToIndex_[lastEntityID] = entityComponentIndex;
    }

}

#endif //SANDBOX_COMPONENT_MANAGER_TPP