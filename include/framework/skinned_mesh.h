
#ifndef SANDBOX_SKINNED_MESH_H
#define SANDBOX_SKINNED_MESH_H

#include <sandbox_pch.h>
#include <framework/mesh.h>
#include <framework/bone.h>

namespace Sandbox {

    class SkinnedMesh : public Mesh {
        public:
            SkinnedMesh(GLuint renderingPrimitive = -1u);
            ~SkinnedMesh() override;

            void SetBoneIDs(const std::vector<glm::vec4>& boneIDs);
            void SetBoneWeights(const std::vector<glm::vec4>& boneWeights);

            [[nodiscard]] const std::vector<glm::vec4>& GetBoneIDs() const;
            [[nodiscard]] const std::vector<glm::vec4>& GetBoneWeights() const;

        private:
            void InitializeBuffers() override;

            // Maximum number of 4 (affecting) bones per vertex.
            // TODO: More influencing bones -> requires vector<int>/vector<float> respectively.
            std::vector<glm::vec4> _boneIDs;   // Bone indices that will influence this vertex.
            std::vector<glm::vec4> _boneWeights; // Weights from each bone.
    };

}

#endif //SANDBOX_SKINNED_MESH_H
