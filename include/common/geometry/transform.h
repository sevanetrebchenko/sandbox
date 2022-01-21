
#ifndef SANDBOX_TRANSFORM_H
#define SANDBOX_TRANSFORM_H

#include "pch.h"

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

            void SetRotation(glm::vec3 rotation);
            [[nodiscard]] glm::vec3 GetRotation() const;

            [[nodiscard]] glm::mat4 GetMatrix();

            [[nodiscard]] bool IsDirty() const;

        private:
            void CalculateMatrix();

            bool _isDirty;
            glm::mat4 _matrix;

            glm::vec3 _rotation;
            glm::vec3 _scale;
            glm::vec3 _position;
    };

}

#endif //SANDBOX_TRANSFORM_H
