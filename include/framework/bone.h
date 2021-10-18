
#ifndef SANDBOX_BONE_H
#define SANDBOX_BONE_H

#include <sandbox_pch.h>
#include <framework/vqs.h>

namespace Sandbox {

    struct Bone {
        Bone() : _index(-1),
                 _parentIndex(-1)
                 {
        }

        std::string _name;
        int _index;
        int _parentIndex;

        std::vector<int> _children;

        glm::mat4 _modelToBoneMatrix; // Bind matrix.
        VQS _modelToBoneVQS;

        glm::mat4 _boneToModelMatrix;
        VQS _boneToModelVQS;
    };

}

#endif //SANDBOX_BONE_H
