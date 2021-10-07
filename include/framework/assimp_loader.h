
#ifndef SANDBOX_ASSIMP_LOADER
#define SANDBOX_ASSIMP_LOADER

#include <sandbox_pch.h>
#include <framework/skinned_mesh.h>
#include <framework/bone.h>

namespace Sandbox {

    class AssimpLoader {
        public:
            static AssimpLoader& GetInstance();

            [[nodiscard]] SkinnedMesh* LoadFromFile(const std::string& filepath);

        private:
            AssimpLoader();
            ~AssimpLoader();

            void ProcessMesh(aiMesh* mesh, SkinnedMesh* skinnedMesh);
    };

}

#endif//SANDBOX_ASSIMP_LOADER