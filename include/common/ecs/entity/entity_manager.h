
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

            [[nodiscard]] bool Exists(int entityID) const;
            [[nodiscard]] bool Exists(const std::string& entityName) const;

            void DestroyEntity(int entityID);
            void DestroyEntity(const std::string& entityName);

            [[nodiscard]] int GetNamedEntityID(const std::string& entityName) const;

            [[nodiscard]] std::vector<int> GetEntityList() const;

        private:
            [[nodiscard]] std::string GetUniqueEntityName(const std::string& entityName) const;

            int entityCounter_;
            std::stack<int> recycledEntityIDs_;

            // Stores mappings for easier ID/name lookup.
            std::unordered_map<int, std::string> IDToName_;
            std::unordered_map<std::string, int> nameToID_;

            // List of all valid entity IDs.
            std::vector<int> entityIDs_;
    };

}

#endif //SANDBOX_ENTITY_MANAGER_H
