
#ifndef SANDBOX_OBJECT_LOADER_H
#define SANDBOX_OBJECT_LOADER_H

#include <sandbox.h>

#include <framework/mesh.h>
#include <framework/singleton.h>

namespace Sandbox {

    class OBJLoader : public Singleton<OBJLoader> {
        public:
            REGISTER_SINGLETON(OBJLoader);

            void Initialize() override;
            void Shutdown() override;

            Mesh LoadFromFile(const std::string& objFilePath);

        private:
            OBJLoader();
            ~OBJLoader() override;

            void MinMaxVertex(const glm::vec3& vertex, glm::vec3& minimum, glm::vec3& maximum) const;
            void TransformToOrigin(std::vector<glm::vec3>& vertices, const glm::vec3& minimum, const glm::vec3& maximum) const;
            void ScaleToUniform(std::vector<glm::vec3>& vertices, const glm::vec3& minimum, const glm::vec3& maximum) const;

            std::unordered_map<std::string, Mesh> _loadedMeshes;
    };

}

#endif //SANDBOX_OBJECT_LOADER_H
