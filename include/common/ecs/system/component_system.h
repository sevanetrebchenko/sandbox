
#ifndef SANDBOX_COMPONENT_SYSTEM_H
#define SANDBOX_COMPONENT_SYSTEM_H

#include "pch.h"

namespace Sandbox {

    // Interface for making systems polymorphic at runtime.
    class IComponentSystem {
        public:
            virtual void Init();
            virtual void Update() = 0;
            virtual void Shutdown();

            // Queries the components attached to the given entity to determine if it should be processed by this system.
            // Returns true if the entity has all the necessary components to be processed by this system.
            [[nodiscard]] virtual bool CheckEntityComponents(int entityID) = 0;

            void AddEntity(int entityID);
            void RemoveEntity(int entityID);
            [[nodiscard]] bool ManagesEntity(int entityID) const;

        protected:
            // Use ECS helper functions to operate on entity components.
            std::vector<int> entityIDs_;
    };

}

#endif //SANDBOX_COMPONENT_SYSTEM_H
