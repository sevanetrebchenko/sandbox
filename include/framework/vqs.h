
#ifndef SANDBOX_VQS_H
#define SANDBOX_VQS_H

#include <sandbox_pch.h>
#include <framework/quaternion.h>

namespace Sandbox {

    class VQS {
        public:
            VQS();
            VQS(const glm::vec3& translation, const Quaternion& orientation, float scalingFactor = 1.0f);
            ~VQS();

            [[nodiscard]] const glm::vec3& GetTranslation() const;
            [[nodiscard]] const Quaternion& GetOrientation() const;
            [[nodiscard]] float GetScalingFactor() const;

            void SetTranslation(const glm::vec3& translation);
            void SetOrientation(const Quaternion& quaternion);
            void SetScalingFactor(float scalingFactor);

            // Concatenation of two VQS structures.
            [[nodiscard]] VQS operator*(const VQS& other) const;

            // Rotation of a 3D vector using VQS structure.
            [[nodiscard]] glm::vec3 operator*(const glm::vec3& other) const;

        private:
            glm::vec3 _v;  // Translation.
            Quaternion _q; // Orientation.
            float _s;      // Uniform scaling value.
    };

}

#endif //SANDBOX_VQS_H
