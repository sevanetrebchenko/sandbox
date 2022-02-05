
#include "common/ecs/iterator/entity_component_iterator.h"

namespace Sandbox {

    ECSAction::ECSAction(ECSAction::Type type, int entityID, const ComponentList& componentList) : entityID_(entityID),
                                                                                                   type_(type),
                                                                                                   entityComponents_(componentList)
                                                                                                   {
    }

    ECSAction::~ECSAction() {
    }

    IEntityComponentIterator::IEntityComponentIterator() : initialized_(false) {
    }

    IEntityComponentIterator::~IEntityComponentIterator() {
    }

    void IEntityComponentIterator::ApplyAction(const ECSAction& action) {
        switch (action.type_) {
            case ECSAction::ENTITY_CREATE: {
                // Created entities only have a Transform component.
                const ComponentList& componentList = action.entityComponents_;

                if (ValidateEntityComponentIDs(componentList)) {
                    // Processes only transform.
                    validEntityList_.emplace(action.entityID_);
                }
                break;
            }

            case ECSAction::ENTITY_DESTROY: {
                auto iterator = validEntityList_.find(action.entityID_);
                if (iterator != validEntityList_.end()) {
                    validEntityList_.erase(iterator);
                }
                break;
            }

            case ECSAction::COMPONENT_ADD: {
                // At this point, the component has been added to the entity.
                auto iterator = validEntityList_.find(action.entityID_);
                if (iterator == validEntityList_.end()) {
                    // Entity does not already exist in the valid entity list.
                    // Check entity components.
                    if (ValidateEntityComponentIDs(action.entityComponents_)) {
                        validEntityList_.emplace(action.entityID_);
                    }
                }
                break;
            }

            case ECSAction::COMPONENT_REMOVE: {
                // At this point, the component has been removed from the entity.
                auto iterator = validEntityList_.find(action.entityID_);
                if (iterator != validEntityList_.end()) {
                    // Entity exists in the valid entity list.
                    // Check entity components.
                    if (!ValidateEntityComponentIDs(action.entityComponents_)) {
                        validEntityList_.erase(iterator);
                    }
                }
                break;
            }

        }
    }

    const std::unordered_set<int>& IEntityComponentIterator::GetValidEntityList() {
        return validEntityList_;
    }

    void IEntityComponentIterator::Reset() {
        validEntityList_.clear();
        initialized_ = false;
    }

    bool IEntityComponentIterator::Initialized() const {
        return initialized_;
    }

}