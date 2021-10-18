
#include <framework/quaternion.h>

namespace Sandbox {

    Quaternion::Quaternion() : _s(1.0f),
                               _v(0.0f)
                               {
        // Identity quaternion.
    }

    Quaternion::Quaternion(float s, float x, float y, float z) : _s(s),
                                                                 _v(x, y, z)
                                                                 {

    }

    Quaternion::Quaternion(float s, const glm::vec3 &v) : _s(s),
                                                          _v(v)
                                                          {
    }

    Quaternion::Quaternion(const glm::mat4 &matrix) {
        _s = 0.5f * std::sqrt(matrix[0][0] + matrix[1][1] + matrix[2][2] + 1.0f);

        float denominator = 1.0f / (4.0f * _s);
        float x = (matrix[1][2] - matrix[2][1]) * denominator;
        float y = (matrix[2][0] - matrix[0][2]) * denominator;
        float z = (matrix[0][1] - matrix[1][0]) * denominator;

        _v = glm::vec3(x, y, z);
    }

    Quaternion::~Quaternion() = default;

    Quaternion Quaternion::Identity() {
        static Quaternion identity(1.0f, glm::vec3(0.0f));
        return identity;
    }

    Quaternion Quaternion::operator+(const Quaternion &other) const {
        return { _s + other._s, _v + other._v };
    }

    Quaternion &Quaternion::operator+=(const Quaternion &other) {
        _s += other._s;
        _v += other._v;

        return *this;
    }

    Quaternion Quaternion::operator-(const Quaternion &other) const {
        return { _s - other._s, _v - other._v };
    }

    Quaternion &Quaternion::operator-=(const Quaternion &other) {
        _s -= other._s;
        _v -= other._v;

        return *this;
    }

    Quaternion Quaternion::operator*(float multiplier) const {
        return { _s * multiplier, _v * multiplier };
    }

    Quaternion &Quaternion::operator*=(float multiplier) {
        _s *= multiplier;
        _v *= multiplier;

        return *this;
    }

    Quaternion& Quaternion::Normalize() {
        float inverseLength = 1.0f / static_cast<float>(std::sqrt((_s * _s) + glm::dot(_v, _v)));
        _s *= inverseLength;
        _v *= inverseLength;

        return *this;
    }

    Quaternion Quaternion::GetNormalized() const {
        float inverseLength = 1.0f / static_cast<float>(std::sqrt((_s * _s) + glm::dot(_v, _v)));
        return { _s * inverseLength, _v * inverseLength };
    }

    Quaternion& Quaternion::Invert() {
        float inverseLengthSquared = (_s * _s) + glm::dot(_v, _v);

        _s *= inverseLengthSquared;

        // Negate vector portion.
        _v *= -inverseLengthSquared;

        return *this;
    }

    Quaternion Quaternion::GetInverted() const {
        float inverseLengthSquared = (_s * _s) + glm::dot(_v, _v);
        return { _s * inverseLengthSquared, _v * -inverseLengthSquared };
    }

    float Quaternion::Dot(const Quaternion &other) const {
        return _s * other._s + glm::dot(_v, other._v);
    }

    Quaternion Quaternion::operator*(const Quaternion &other) const {
        float scalar = _s * other._s - glm::dot(_v, other._v);
        glm::vec3 vector = _s * other._v + other._s * _v + glm::cross(_v, other._v);

        return { scalar, vector };
    }

    Quaternion Quaternion::Conjugate() const {
        return { _s, -_v };
    }

    glm::vec3 Quaternion::operator*(const glm::vec3 &vector) const {
        return { ((2.0f * _s * _s - 1.0f) * vector) + (2.0f * glm::dot(_v, vector) * _v) + (2.0f * _s * glm::cross(_v, vector)) };
    }

    glm::mat4 Quaternion::ToMatrix() const {
        glm::mat4 matrix(1.0f);

        float s = _s;
        float x = _v.x;
        float y = _v.y;
        float z = _v.z;

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

    Quaternion Quaternion::Slerp(const Quaternion &start, const Quaternion &end, float t) {
        return Quaternion(0, 0, 0, 0); // TODO;
    }

    Quaternion operator*(float multiplier, const Quaternion &quaternion) {
        return quaternion * multiplier;
    }

}
