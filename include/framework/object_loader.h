
#ifndef SANDBOX_OBJECT_LOADER_H
#define SANDBOX_OBJECT_LOADER_H

#include <sandbox_pch.h>
#include <framework/mesh.h>

namespace Sandbox {

    class OBJLoader {
        public:
            static OBJLoader& GetInstance();

            Mesh LoadFromFile(const std::string& objFilePath);

        private:
            OBJLoader();
            ~OBJLoader();

            glm::vec3 ReadVertexPosition(std::stringstream& stringParser) const;
            std::vector<unsigned> ReadFace(std::stringstream& stringParser) const;

            void MinMaxVertex(const glm::vec3& vertex, glm::vec3& minimum, glm::vec3& maximum) const;

            std::unordered_map<std::string, Mesh> _loadedMeshes;
    };

}

#endif //SANDBOX_OBJECT_LOADER_H
