
#ifndef SANDBOX_SKELETON_H
#define SANDBOX_SKELETON_H

#include <sandbox_pch.h>
#include <framework/bone.h>

namespace Sandbox {

    struct Skeleton {
        Skeleton() {}
        ~Skeleton() {}

        void AddBone(const Bone& bone) {
            _boneMapping[bone._name] = _bones.size();
            _bones.push_back(bone);
        }

        void AddRoot(int root) {
            _roots.push_back(root);
        }

        std::vector<Bone> _bones;
        std::vector<int> _roots;
        std::unordered_map<std::string, unsigned> _boneMapping;
    };

}

#endif //SANDBOX_SKELETON_H
