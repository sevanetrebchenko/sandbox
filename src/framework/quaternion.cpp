
#include <framework/quaternion.h>

namespace Sandbox {

    Quaternion::Quaternion() : quat(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
                               {
        // Identity quaternion.
    }

    Quaternion::Quaternion(float s, float x, float y, float z) : quat(x, y, z, s)
                                                                 {

    }

    Quaternion::Quaternion(const glm::mat4 &matrix) {
        quat.w = 0.5f * std::sqrt(matrix[0][0] + matrix[1][1] + matrix[2][2] + 1.0f);

        float denominator = 1.0f / (4.0f * quat.w);
        quat.x = (matrix[1][2] - matrix[2][1]) * denominator;
        quat.y = (matrix[2][0] - matrix[0][2]) * denominator;
        quat.z = (matrix[0][1] - matrix[1][0]) * denominator;
    }

    Quaternion::Quaternion(const glm::vec3 &axis, float angle) {
        float radians = glm::radians(angle);

        glm::vec3 a = glm::normalize(axis);
        float s = glm::sin(0.5f * radians);

        quat.w = glm::cos(0.5f * radians);
        quat.x = a.x * s;
        quat.y = a.y * s;
        quat.z = a.z * s;
    }

    Quaternion::~Quaternion() = default;

    Quaternion Quaternion::Identity() {
        static Quaternion identity(1.0f, 0.0f, 0.0f, 0.0f);
        return identity;
    }

    float Quaternion::GetScalar() const {
        return quat.w;
    }

    glm::vec3 Quaternion::GetVector() const {
        return { quat.x, quat.y, quat.z };
    }

    Quaternion Quaternion::operator+(const Quaternion &other) const {
        float x = quat.x + other.quat.x;
        float y = quat.y + other.quat.y;
        float z = quat.z + other.quat.z;
        float w = quat.w + other.quat.w;

        return { w, x, y, z };
    }

    Quaternion &Quaternion::operator+=(const Quaternion &other) {
        quat.x += other.quat.x;
        quat.y += other.quat.y;
        quat.z += other.quat.z;
        quat.w += other.quat.w;

        return *this;
    }

    Quaternion Quaternion::operator-(const Quaternion &other) const {
        float x = quat.x - other.quat.x;
        float y = quat.y - other.quat.y;
        float z = quat.z - other.quat.z;
        float w = quat.w - other.quat.w;

        return { w, x, y, z };
    }

    Quaternion &Quaternion::operator-=(const Quaternion &other) {
        quat.x -= other.quat.x;
        quat.y -= other.quat.y;
        quat.z -= other.quat.z;
        quat.w -= other.quat.w;

        return *this;
    }

    Quaternion Quaternion::operator*(float multiplier) const {
        float x = quat.x * multiplier;
        float y = quat.y * multiplier;
        float z = quat.z * multiplier;
        float w = quat.w * multiplier;

        return { w, x, y, z };
    }

    Quaternion &Quaternion::operator*=(float multiplier) {
        quat.x *= multiplier;
        quat.y *= multiplier;
        quat.z *= multiplier;
        quat.w *= multiplier;

        return *this;
    }

    Quaternion Quaternion::Normalize(const Quaternion& quaternion) {
        float x = quaternion.quat.x;
        float y = quaternion.quat.y;
        float z = quaternion.quat.z;
        float w = quaternion.quat.w;

        float inverseLength = 1.0f / glm::sqrt(w * w + x * x + y * y + z * z);
        return { w * inverseLength, x * inverseLength, y * inverseLength, z * inverseLength };
    }

    Quaternion Quaternion::operator*(const Quaternion &other) const {
        glm::vec3 v0 = GetVector();
        glm::vec3 v1 = other.GetVector();

        float scalar = quat.w * other.quat.w - glm::dot(v0, v1);
        glm::vec3 vector = quat.w * v1 + other.quat.w * v0 + glm::cross(v0, v1);

        return { scalar, vector.x, vector.y, vector.z };
    }

    Quaternion Quaternion::Conjugate(const Quaternion& quaternion) {
        return { quaternion.quat.w, -quaternion.quat.x, -quaternion.quat.y, -quaternion.quat.z };
    }

    float Quaternion::DotProduct(const Quaternion& first, const Quaternion &second) {
        return first.quat.w * second.quat.w + first.quat.x * second.quat.x + first.quat.y * second.quat.y + first.quat.z * second.quat.z;
    }

    glm::vec3 Quaternion::operator*(const glm::vec3 &vector) const {
        glm::vec3 v = GetVector();
        return { ((2.0f * quat.w * quat.w - 1.0f) * vector) + (2.0f * glm::dot(v, vector) * v) + (2.0f * quat.w * glm::cross(v, vector)) };
    }

    glm::mat4 Quaternion::ToMatrix() const {
        glm::mat4 matrix(1.0f);

        float s = quat.w;
        float x = quat.x;
        float y = quat.y;
        float z = quat.z;

        float xx = x * x;
        float xy = x * y;
        float xz = x * z;

        float yy = y * y;
        float yz = y * z;

        float zz = z * z;

        float sx = s * x;
        float sy = s * y;
        float sz = s * z;

        // Column major.
        matrix[0][0] = 1.0f - 2.0f * (yy + zz);
        matrix[0][1] = 2.0f * (xy + sz);
        matrix[0][2] = 2.0f * (xz - sy);

        matrix[1][0] = 2.0f * (xy - sz);
        matrix[1][1] = 1.0f - 2.0f * (xx + zz);
        matrix[1][2] = 2.0f * (yz + sx);

        matrix[2][0] = 2.0f * (xz + sy);
        matrix[2][1] = 2.0f * (yz - sx);
        matrix[2][2] = 1.0f - 2.0f * (xx + yy);

        // Translation portion is default initialized to [0, 0, 0, 1].
        matrix[3][0] = 0.0f;
        matrix[3][1] = 0.0f;
        matrix[3][2] = 0.0f;
        matrix[3][3] = 1.0f;

        return matrix;
    }

    glm::vec4 Quaternion::ToVec4() const {
        return { quat.x, quat.y, quat.z, quat.w };
    }

    Quaternion Quaternion::operator/(float divisor) const {
        return this->operator*(1.0f / divisor);
    }

    Quaternion &Quaternion::operator/=(float divisor) {
        return this->operator*=(1.0f / divisor);
    }

    Quaternion operator*(float multiplier, const Quaternion &quaternion) {
        return quaternion * multiplier;
    }

    Quaternion operator/(float divisor, const Quaternion &quaternion) {
        return quaternion / divisor;
    }

    glm::vec3 operator*(const glm::vec3 &vector, const Quaternion &quaternion) {
        return quaternion * vector;
    }

}
