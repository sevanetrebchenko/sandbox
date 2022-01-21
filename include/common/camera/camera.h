
#ifndef SANDBOX_CAMERA_H
#define SANDBOX_CAMERA_H

#include "pch.h"

namespace Sandbox {

    class ICamera {
        public:
            ICamera(int width, int height);
            virtual ~ICamera();

            // Should be called every frame.
            virtual void Update() = 0;

            void SetPosition(const glm::vec3& position);

            // Look-at vector.
            void SetTargetPosition(const glm::vec3& position);
            void SetLookAtDirection(const glm::vec3& direction);

            void SetNearPlaneDistance(float distance);
            void SetFarPlaneDistance(float distance);

            void SetFOVAngle(float fov);

            void SetAspectRatio(float width, float height);
            void SetAspectRatio(float aspectRatio);

            // Assumes degrees.
            void SetEulerAngles(const glm::vec3& eulerAngles);
            void SetEulerAngles(float pitch, float yaw, float roll);

            [[nodiscard]] const glm::vec3& GetPosition() const;

            // View * Perspective.
            [[nodiscard]] const glm::mat4& GetCameraTransform();
            [[nodiscard]] const glm::mat4& GetPerspectiveTransform();
            [[nodiscard]] const glm::mat4& GetViewTransform();

            [[nodiscard]] const glm::vec3& GetForwardVector() const;
            [[nodiscard]] const glm::vec3& GetUpVector() const;

            // Euler angles are stored in radians.
            [[nodiscard]] const glm::vec3& GetEulerAngles() const;
            [[nodiscard]] float GetPitch() const; // x
            [[nodiscard]] float GetYaw() const;   // y
            [[nodiscard]] float GetRoll() const;  // z

            [[nodiscard]] float GetNearPlaneDistance() const;
            [[nodiscard]] float GetFarPlaneDistance() const;

            [[nodiscard]] float GetAspectRatio() const;

        protected:
            void RecalculateMatrices();
            [[nodiscard]] bool IsDirty() const;

            glm::mat4 cameraTransform_;
            glm::mat4 viewTransform_;
            glm::mat4 perspectiveTransform_;

            glm::vec3 position_;
            glm::vec3 lookAtDirection_;
            glm::vec3 up_;

            glm::vec3 eulerAngles_;

            float fov_;
            float aspectRatio_;

            float nearPlaneDistance_;
            float farPlaneDistance_;

            bool isDirty_;
    };

}

#endif //SANDBOX_CAMERA_H
