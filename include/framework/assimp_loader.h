
#ifndef SANDBOX_ASSIMP_LOADER
#define SANDBOX_ASSIMP_LOADER

#include <sandbox_pch.h>
#include <framework/skinned_mesh.h>

namespace Sandbox {

    class AssimpLoader {
        public:
            static AssimpLoader& GetInstance();

            [[nodiscard]] SkinnedMesh LoadFromFile(const std::string& filepath);

        private:
            AssimpLoader();
            ~AssimpLoader();

            void ProcessMesh(aiMesh* mesh, SkinnedMesh& skinnedMesh);

            void MinMaxVertex(const glm::vec3& vertex, glm::vec3& minimum, glm::vec3& maximum) const;
            glm::vec3 GetGLMVector(const aiVector3D& input) const;
            glm::mat4 GetGLMMatrix(const aiMatrix4x4& input) const;

            std::unordered_map<std::string, SkinnedMesh> _loadedMeshes;
    };

}

#endif//SANDBOX_ASSIMP_LOADER