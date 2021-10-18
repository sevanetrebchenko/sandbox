
#ifndef SANDBOX_QUATERNION_H
#define SANDBOX_QUATERNION_H

#include <sandbox_pch.h>

namespace Sandbox {

    class Quaternion {
        public:
            Quaternion();
            Quaternion(float s, float x, float y, float z);
            Quaternion(float s, const glm::vec3& v);
            Quaternion(const glm::mat4& matrix);
            ~Quaternion();

            [[nodiscard]] static Quaternion Identity();

            [[nodiscard]] float GetScalar() const;
            [[nodiscard]] glm::vec3 GetVector() const;

            // Arithmetic operations with other Quaternions.
            [[nodiscard]] Quaternion operator+(const Quaternion& other) const;
            Quaternion& operator+=(const Quaternion& other);
            [[nodiscard]] Quaternion operator-(const Quaternion& other) const;
            Quaternion& operator-=(const Quaternion& other);
            [[nodiscard]] Quaternion operator*(float multiplier) const;
            Quaternion& operator*=(float multiplier);
            [[nodiscard]] Quaternion operator/(float divisor) const;
            Quaternion& operator/=(float divisor);

            [[nodiscard]] static Quaternion Normalize(const Quaternion& quaternion);
            // [[nodiscard]] static Quaternion Invert(const Quaternion& quaternion);
            [[nodiscard]] static Quaternion Conjugate(const Quaternion& quaternion);
            [[nodiscard]] static float DotProduct(const Quaternion& first, const Quaternion& second);

            [[nodiscard]] Quaternion operator*(const Quaternion& other) const; // Quaternion product.

            // Rotate input vector by this Quaternion.
            [[nodiscard]] glm::vec3 operator*(const glm::vec3& vector) const;

            // Converter functions.
            [[nodiscard]] glm::mat4 ToMatrix() const;
            [[nodiscard]] glm::vec4 ToVec4() const;

        private:
            // x, y, z, w
            glm::vec4 quat;
    };

    [[nodiscard]] Quaternion operator*(float multiplier, const Quaternion& quaternion);
    [[nodiscard]] Quaternion operator/(float divisor, const Quaternion& quaternion);
    [[nodiscard]] glm::vec3 operator*(const glm::vec3& vector, const Quaternion& quaternion);

}

#endif //SANDBOX_QUATERNION_H
