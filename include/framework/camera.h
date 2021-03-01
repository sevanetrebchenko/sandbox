
#ifndef SANDBOX_CAMERA_H
#define SANDBOX_CAMERA_H

#include <sandbox.h>

namespace Sandbox {

    class Camera {
        public:
            Camera(float width, float height);

            ~Camera();

            [[nodiscard]] const glm::vec3& GetForwardVector() const;
            [[nodiscard]] const glm::vec3& GetUpVector() const;

            void SetEyePosition(glm::vec3 position);
            glm::vec3& GetEyePosition();

            void SetLookAtDirection(glm::vec3 direction);
            glm::vec3& GetLookAtDirection();

            void SetUpVector(glm::vec3 upVector);
            glm::vec3& GetUpVector();

            void SetLastMousePosition(glm::vec2 mousePos);
            glm::vec2& GetLastMousePosition();

            void SetCameraSensitivity(float sensitivity);
            float& GetCameraSensitivity();

            void SetCameraYaw(float yaw);
            float& GetCameraYaw();

            void SetCameraPitch(float yaw);
            float& GetCameraPitch();

            void SetCursorEnabled(bool enabled);
            bool& GetCursorEnabled();

            const glm::mat4& GetMatrix();

            const glm::mat4& GetPerspectiveMatrix();
            const glm::mat4& GetViewMatrix();

            [[nodiscard]] float GetNearPlaneDistance() const;
            void SetNearPlaneDistance(float nearPlaneDistance);

            [[nodiscard]] float GetFarPlaneDistance() const;
            void SetFarPlaneDistance(float farPlaneDistance);

            [[nodiscard]] float GetFOV() const;
            void SetFOV(float FOV);

            [[nodiscard]] float GetAspectRatio() const;
            void SetAspectRatio(float aspectRatio);

        private:
            void CalculateMatrix();

            bool _isDirty;

            glm::vec3 _eyePosition;
            glm::vec3 _lookAtDirection;
            glm::vec3 _upVector;

            float _width;
            float _height;
            float _fovAngle;
            float _aspectRatio;
            float _nearPlaneDistance;
            float _farPlaneDistance;

            glm::vec2 _mousePosition;
            float _cameraSensitivity;
            float _yaw;
            float _pitch;
            bool _cursorEnabled;

            glm::mat4 _cameraMatrix;
            glm::mat4 _perspectiveMatrix;
            glm::mat4 _viewMatrix;
    };

}

#endif //SANDBOX_CAMERA_H
