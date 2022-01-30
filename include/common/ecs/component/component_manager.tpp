
#ifndef SANDBOX_COMPONENT_MANAGER_TPP
#define SANDBOX_COMPONENT_MANAGER_TPP

namespace Sandbox {

    template <typename T>
    ComponentManager<T>::ComponentManager() : IComponentManager() {
    }

    template<typename T>
    ComponentManager<T>::~ComponentManager() {
    }

    template<typename T>
    void ComponentManager<T>::Reset() {
        for (T* component : components_) {
            delete component;
        }
        components_.clear();

        IDToIndex_.clear();
        indexToID_.clear();
    }

    template<typename T>
    template <typename ...Args>
    T* ComponentManager<T>::AddComponent(int entityID, const Args&... args) {
        auto iterator = IDToIndex_.find(entityID);
        if (iterator != IDToIndex_.end()) {
            // Component already exists at this entity ID.
            throw std::runtime_error("From ComponentManager<T>::AddComponent: Component already exists at the given entity ID.");
        }

        int index = components_.size();
        components_.emplace_back(new T(args...));

        // Register mapping.
        IDToIndex_.emplace(entityID, index);
        indexToID_.emplace(index, entityID);

        return components_[index];
    }

    template <typename T>
    template <typename Fn, typename ...Args>
    void ComponentManager<T>::SetComponent(int entityID, const Args& ...args, Fn&& callback) {
        T* component;

        if (HasComponent(entityID)) {
            component = GetComponent(entityID);
        }
        else {
            component = AddComponent(entityID, args...);
        }

        callback(*component);
    }

    template<typename T>
    T* ComponentManager<T>::GetComponent(int entityID) const {
        auto iterator = IDToIndex_.find(entityID);
        if (iterator != IDToIndex_.end()) {
            return components_[iterator->second];
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

        int toRemoveIndex = indexIterator->second;

        // Erase deleted component mapping.
        IDToIndex_.erase(entityID);

        int lastIndex = components_.size() - 1; // Guaranteed to be positive.
        int lastEntityID = indexToID_[lastIndex];

        std::swap(components_[toRemoveIndex], components_[lastIndex]);
        components_.pop_back(); // Delete component.

        indexToID_.erase(lastIndex);

        // Update mapping.
        indexToID_[toRemoveIndex] = lastEntityID;
        IDToIndex_[lastEntityID] = toRemoveIndex;
    }

}

#endif //SANDBOX_COMPONENT_MANAGER_TPP