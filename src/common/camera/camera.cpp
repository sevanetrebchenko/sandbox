
#include "common/camera/camera.h"
#include "common/api/backend.h"

namespace Sandbox {

    ICamera::ICamera(int width, int height) : cameraTransform_(1.0f),
                                              viewTransform_(1.0f),
                                              perspectiveTransform_(1.0f),
                                              position_(0.0f, 0.0f, 0.0f),
                                              lookAtDirection_(0.0f, 0.0f, -1.0f),
                                              up_(0.0f, 1.0f, 0.0f),
                                              eulerAngles_(0.0f, -glm::pi<float>() / 2.0f, 0.0f),
                                              fov_(75.0),
                                              aspectRatio_((float) width / (float) height),
                                              nearPlaneDistance_(0.0001f),
                                              farPlaneDistance_(40.0f),
                                              isDirty_(true) {
    }

    ICamera::~ICamera() = default;

    void ICamera::SetPosition(const glm::vec3 &position) {
        position_ = position;
        isDirty_ = true;
    }

    void ICamera::SetTargetPosition(const glm::vec3 &position) {
        lookAtDirection_ = glm::normalize(position - position_);
        isDirty_ = true;
    }

    void ICamera::SetLookAtDirection(const glm::vec3 &direction) {
        lookAtDirection_ = glm::normalize(direction);
        isDirty_ = true;
    }

    void ICamera::SetNearPlaneDistance(float distance) {
        nearPlaneDistance_ = distance;
        isDirty_ = true;
    }

    void ICamera::SetFarPlaneDistance(float distance) {
        farPlaneDistance_ = distance;
        isDirty_ = true;
    }

    void ICamera::SetFOVAngle(float fov) {
        fov_ = fov;
        isDirty_ = true;
    }

    void ICamera::SetAspectRatio(float width, float height) {
        aspectRatio_ = width / height;
        isDirty_ = true;
    }

    void ICamera::SetAspectRatio(float aspectRatio) {
        aspectRatio_ = aspectRatio;
        isDirty_ = true;
    }

    void ICamera::SetEulerAngles(const glm::vec3 &eulerAngles) {
        eulerAngles_ = glm::radians(eulerAngles);

        float pitch = eulerAngles_.x;
        float yaw = eulerAngles_.y;
        float roll = eulerAngles_.z;

        glm::vec3 forwardVector;
        forwardVector.x = glm::cos(yaw) * glm::cos(pitch);
        forwardVector.y = glm::sin(pitch);
        forwardVector.z = glm::sin(yaw) * glm::cos(pitch);

        // Set forward vector.
        SetLookAtDirection(forwardVector);
    }

    void ICamera::SetEulerAngles(float pitch, float yaw, float roll) {
        SetEulerAngles(glm::vec3(pitch, yaw, roll));
    }

    const glm::vec3 &ICamera::GetPosition() const {
        return position_;
    }

    const glm::mat4 &ICamera::GetCameraTransform() {
        if (isDirty_) {
            RecalculateMatrices();
        }
        return cameraTransform_;
    }

    const glm::mat4 &ICamera::GetPerspectiveTransform() {
        if (isDirty_) {
            RecalculateMatrices();
        }
        return perspectiveTransform_;
    }

    const glm::mat4 &ICamera::GetViewTransform() {
        if (isDirty_) {
            RecalculateMatrices();
        }
        return viewTransform_;
    }

    const glm::vec3 &ICamera::GetForwardVector() const {
        return lookAtDirection_;
    }

    const glm::vec3 &ICamera::GetUpVector() const {
        return up_;
    }

    const glm::vec3 &ICamera::GetEulerAngles() const {
        return eulerAngles_;
    }

    float ICamera::GetPitch() const {
        return eulerAngles_.x;
    }

    float ICamera::GetYaw() const {
        return eulerAngles_.y;
    }

    float ICamera::GetRoll() const {
        return eulerAngles_.z;
    }

    float ICamera::GetNearPlaneDistance() const {
        return nearPlaneDistance_;
    }

    bool ICamera::IsDirty() const {
        return isDirty_;
    }

    float ICamera::GetAspectRatio() const {
        return aspectRatio_;
    }

    float ICamera::GetFarPlaneDistance() const {
        return farPlaneDistance_;
    }

    void ICamera::RecalculateMatrices() {
        viewTransform_ = glm::lookAt(position_, position_ + lookAtDirection_, up_);
        perspectiveTransform_ = glm::perspective(glm::radians(fov_), aspectRatio_, nearPlaneDistance_, farPlaneDistance_);
        cameraTransform_ = perspectiveTransform_ * viewTransform_;
        isDirty_ = false;
    }

    float ICamera::GetFOV() const {
        return fov_;
    }

}
