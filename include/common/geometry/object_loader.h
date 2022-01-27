
#ifndef SANDBOX_OBJECT_LOADER_H
#define SANDBOX_OBJECT_LOADER_H

#include "pch.h"
#include "mesh.h"

namespace Sandbox {

    class OBJLoader {
        public:
            static OBJLoader& Instance();
            Mesh LoadFromFile(const std::string& objFilePath);

        private:
            OBJLoader();
            ~OBJLoader();

            void MinMaxVertex(const glm::vec3& vertex, glm::vec3& minimum, glm::vec3& maximum) const;
            void TransformToOrigin(std::vector<glm::vec3>& vertices, const glm::vec3& minimum, const glm::vec3& maximum) const;
            void ScaleToUniform(std::vector<glm::vec3>& vertices, const glm::vec3& minimum, const glm::vec3& maximum) const;

            std::unordered_map<std::string, Mesh> _loadedMeshes;
    };

}

#endif //SANDBOX_OBJECT_LOADER_H
