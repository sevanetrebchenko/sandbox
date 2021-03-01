
#ifndef SANDBOX_OBJECT_LOADER_H
#define SANDBOX_OBJECT_LOADER_H

#include <sandbox.h>
#include <framework/mesh.h>

namespace Sandbox {

    class OBJLoader {
        public:
            struct LoadingParameters {
                LoadingParameters(std::string filename, bool normalizePositions, bool normalizeScale);

                std::string filename;
                bool normalizePositions;
                bool normalizeScale;
            };

            static OBJLoader& GetInstance();

            Mesh LoadFromFile(std::string objFilePath, bool normalizePositions = true, bool normalizeScale = true);
            Mesh LoadFromFile(const LoadingParameters& loadingParameters);

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
