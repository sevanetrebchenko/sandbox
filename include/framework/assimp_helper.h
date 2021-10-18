
#ifndef SANDBOX_ASSIMP_HELPER_H
#define SANDBOX_ASSIMP_HELPER_H

#include <sandbox_pch.h>
#include <framework/quaternion.h>

namespace Sandbox {

    glm::vec3 GetGLMVector(const aiVector3D& input);
    glm::mat4 GetGLMMatrix(const aiMatrix4x4& input);
    glm::quat GetGLMQuaternion(const aiQuaternion& input);

    Quaternion GetQuaternion(const aiQuaternion& input);

}

#endif //SANDBOX_ASSIMP_HELPER_H
