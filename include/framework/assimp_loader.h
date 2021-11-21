
#ifndef SANDBOX_ASSIMP_LOADER
#define SANDBOX_ASSIMP_LOADER

#include <sandbox_pch.h>
#include <framework/skinned_mesh.h>
#include <framework/bone.h>
#include <framework/skeleton.h>
#include <framework/animator.h>
#include <framework/animated_model.h>

namespace Sandbox {

    class AssimpLoader {
        public:
            static AssimpLoader& GetInstance();

            [[nodiscard]] AnimatedModel* LoadFromFile(const std::string& modelName, const std::string& filepath);

        private:
            AssimpLoader();
            ~AssimpLoader();

            void LoadMeshData(const aiMesh* mesh, SkinnedMesh* meshData);
            void LoadSkeletonData(const aiMesh* mesh, SkinnedMesh* meshData, Skeleton* skeleton);
            void LoadSceneAnimations(const aiScene* scene, Skeleton* skeleton, Animator* animator);
    };

}

#endif//SANDBOX_ASSIMP_LOADER