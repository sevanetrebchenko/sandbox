
#include "common/camera/fps_camera.h"
#include "common/application/time.h"
#include "common/api/window.h"
#include "common/application/input.h"

namespace Sandbox {

    FPSCamera::FPSCamera(int width, int height) : ICamera(width, height),
                                                  cameraSpeed_(10.0f)
                                                  {
    }

    FPSCamera::~FPSCamera() {
    }

    void FPSCamera::Update() {
        CameraMovement();
        LookAround();
    }

    void FPSCamera::CameraMovement() {
        float dt = Time::Instance().dt;
        Input& input = Input::Instance();

        if (input.IsKeyPressed(GLFW_KEY_W)) {
            SetPosition(position_ + cameraSpeed_ * lookAtDirection_ * dt);
        }

        if (input.IsKeyPressed(GLFW_KEY_S)) {
            SetPosition(position_ - cameraSpeed_ * lookAtDirection_ * dt);
        }

        if (input.IsKeyPressed(GLFW_KEY_A)) {
            SetPosition(position_ - glm::normalize(glm::cross(lookAtDirection_, up_)) * cameraSpeed_ * dt);
        }

        if (input.IsKeyPressed(GLFW_KEY_D)) {
            SetPosition(position_ + glm::normalize(glm::cross(lookAtDirection_, up_)) * cameraSpeed_ * dt);
        }

        if (input.IsKeyPressed(GLFW_KEY_E)) {
            SetPosition(position_ + cameraSpeed_ * up_ * dt);
        }

        if (input.IsKeyPressed(GLFW_KEY_Q)) {
            SetPosition(position_ - cameraSpeed_ * up_ * dt);
        }
    }

    void FPSCamera::LookAround() {
        static glm::vec2 previousCursorPosition;
        static bool initialInput = true;

        Input& input = Input::Instance();

        // Look around while RMB is held.
        if (input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            glm::vec2 cursorPosition = input.GetMouseCursorPosition();

            // FPS camera.
            if (initialInput) {
                previousCursorPosition = cursorPosition;
                initialInput = false;
            }

            float mouseSensitivity = 0.1f;

            float dx = static_cast<float>(cursorPosition.x - previousCursorPosition.x) * mouseSensitivity;
            float dy = static_cast<float>(previousCursorPosition.y - cursorPosition.y) * mouseSensitivity; // Flipped.

            previousCursorPosition = cursorPosition;

            float pitch = glm::degrees(GetPitch());
            float yaw = glm::degrees(GetYaw());
            float roll = glm::degrees(GetRoll());

            float limit = 89.0f;

            yaw += dx;
            pitch += dy;

            if (glm::abs(dx) > std::numeric_limits<float>::epsilon() || glm::abs(dy) > std::numeric_limits<float>::epsilon()) {
                // Prevent camera forward vector to be parallel to camera up vector (0, 1, 0).
                if (pitch > limit) {
                    pitch = limit;
                }
                if (pitch < -limit) {
                    pitch = -limit;
                }

                SetEulerAngles(pitch, yaw, roll);
            }
        }
        else {
            initialInput = true;
        }
    }

    FPSCamera& FPSCamera::operator=(const FPSCamera& other) {
        if (&other == this) {
            return *this;
        }

        cameraSpeed_ = other.cameraSpeed_;

        position_ = other.position_;
        lookAtDirection_ = other.lookAtDirection_;
        up_ = other.up_;

        eulerAngles_ = other.eulerAngles_;

        fov_ = other.fov_;
        aspectRatio_ = other.aspectRatio_;

        nearPlaneDistance_ = other.nearPlaneDistance_;
        farPlaneDistance_ = other.farPlaneDistance_;

        isDirty_ = true;

        return *this;

    }

}
