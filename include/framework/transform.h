
#ifndef SANDBOX_TRANSFORM_H
#define SANDBOX_TRANSFORM_H

#include <sandbox_pch.h>

namespace Sandbox {

    class Transform {
        public:
            Transform();
            ~Transform();

            void OnImGui();

            void SetPosition(glm::vec3 position);
            [[nodiscard]] glm::vec3 GetPosition() const;

            void SetScale(glm::vec3 scale);
            [[nodiscard]] glm::vec3 GetScale() const;

            void SetRotation(float degrees, glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f));
            void SetRotation(glm::quat quaternion);
            [[nodiscard]] glm::quat GetRotation() const;

            void SetRotationOffset(glm::quat rotation);

            [[nodiscard]] glm::mat4 GetMatrix() const;

            [[nodiscard]] bool IsDirty() const;
            void Clean();

        private:
            void CalculateMatrix();

            bool _isDirty;
            glm::mat4 _matrix;

            glm::quat _rotation;
            glm::quat _rotationOffset;

            glm::vec3 _scale;
            glm::vec3 _position;
    };

}

#endif //SANDBOX_TRANSFORM_H
