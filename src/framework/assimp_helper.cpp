
#include <framework/assimp_helper.h>

namespace Sandbox {

    glm::vec3 GetGLMVector(const aiVector3D &input) {
        return { input.x, input.y, input.z };
    }

    glm::mat4 GetGLMMatrix(const aiMatrix4x4 &input) {
        glm::mat4 matrix;

        matrix[0][0] = input.a1; matrix[1][0] = input.a2; matrix[2][0] = input.a3; matrix[3][0] = input.a4;
        matrix[0][1] = input.b1; matrix[1][1] = input.b2; matrix[2][1] = input.b3; matrix[3][1] = input.b4;
        matrix[0][2] = input.c1; matrix[1][2] = input.c2; matrix[2][2] = input.c3; matrix[3][2] = input.c4;
        matrix[0][3] = input.d1; matrix[1][3] = input.d2; matrix[2][3] = input.d3; matrix[3][3] = input.d4;

        return matrix;
    }

    glm::quat GetGLMQuaternion(const aiQuaternion& input) {
        return { input.w, input.x, input.y, input.z };
    }

}
