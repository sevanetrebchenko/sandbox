
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

            // Arithmetic operations with other Quaternions.
            [[nodiscard]] Quaternion operator+(const Quaternion& other) const;
            [[nodiscard]] Quaternion& operator+=(const Quaternion& other);
            [[nodiscard]] Quaternion operator-(const Quaternion& other) const;
            [[nodiscard]] Quaternion& operator-=(const Quaternion& other);
            [[nodiscard]] Quaternion operator*(float multiplier) const;
            [[nodiscard]] Quaternion& operator*=(float multiplier);

            [[nodiscard]] Quaternion& Normalize();
            [[nodiscard]] Quaternion GetNormalized() const;

            [[nodiscard]] Quaternion& Invert();
            [[nodiscard]] Quaternion GetInverted() const;

            [[nodiscard]] float Dot(const Quaternion& other) const;
            [[nodiscard]] Quaternion operator*(const Quaternion& other) const; // Quaternion product.
            [[nodiscard]] Quaternion Conjugate() const; // Quaternion conjugate.

            // Rotate input vector by this Quaternion.
            [[nodiscard]] glm::vec3 operator*(const glm::vec3& vector) const;

            // Convert back to a 4x4 matrix.
            [[nodiscard]] glm::mat4 ToMatrix() const;

            [[nodiscard]] static Quaternion Slerp(const Quaternion& start, const Quaternion& end, float t);

        private:
            float _s;
            glm::vec3 _v;
    };

    [[nodiscard]] Quaternion operator*(float multiplier, const Quaternion& quaternion);

}

#endif //SANDBOX_QUATERNION_H
