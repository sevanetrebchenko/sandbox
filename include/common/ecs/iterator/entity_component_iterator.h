
#pragma once

#include "pch.h"
#include "common/ecs/component/component_list.h"

namespace Sandbox {

    // Action that happened in the ECS system.
    struct ECSAction {
        enum Type {
            ENTITY_CREATE,
            ENTITY_DESTROY,
            COMPONENT_ADD,
            COMPONENT_REMOVE
        };

        ECSAction(Type actionType, int entityID, const ComponentList& componentList);
        ~ECSAction();

        int entityID_;
        ComponentList entityComponents_;
        Type type_;
    };

    class IEntityComponentIterator {
        public:
            IEntityComponentIterator();
            ~IEntityComponentIterator();

            void Reset();

            [[nodiscard]] bool Initialized() const;

            void ApplyAction(const ECSAction& action);
            [[nodiscard]] const std::unordered_set<int>& GetValidEntityList();

        protected:
            // Lazy update valid entity list.
            void Update();

            // Returns true if entity should be processed by this Iterator.
            [[nodiscard]] virtual bool ValidateEntityComponentIDs(const ComponentList& componentList) const = 0;

            bool initialized_;

            std::unordered_set<int> validEntityList_; // Entities that are processed by this Iterator.
            std::stack<ECSAction> actions_;
    };

    template <typename ...T>
    class EntityComponentIterator : public IEntityComponentIterator {
        public:
            EntityComponentIterator();
            ~EntityComponentIterator();

            void Init(const std::vector<std::pair<int, ComponentList>>& state);

        private:
            [[nodiscard]] bool ValidateEntityComponentIDs(const ComponentList& componentList) const override;
    };

}

#include "common/ecs/iterator/entity_component_iterator.tpp"