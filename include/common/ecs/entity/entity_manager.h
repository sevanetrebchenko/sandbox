
#ifndef SANDBOX_ENTITY_MANAGER_H
#define SANDBOX_ENTITY_MANAGER_H

#include "pch.h"

namespace Sandbox {

    class EntityManager {
        public:
            EntityManager();
            ~EntityManager();

            void Reset(); // Clears all entities.

            [[nodiscard]] int CreateEntity(std::string entityName = "");

            void DestroyEntity(int entityID);
            void DestroyEntity(const std::string& entityName);

            [[nodiscard]] int GetNamedEntityID(const std::string& entityName) const;

            [[nodiscard]] std::vector<int> GetEntityList() const;

        private:
            [[nodiscard]] std::string GetUniqueEntityName(const std::string& entityName) const;

            int entityCounter_;
            std::stack<int> recycledEntityIDs_;

            // Provides index into entityIDs vector.
            std::unordered_map<int, std::string> IDToName_;
            std::unordered_map<std::string, int> nameToID_;
    };

}

#endif //SANDBOX_ENTITY_MANAGER_H
