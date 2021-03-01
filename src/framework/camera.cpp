
#include <framework/camera.h>
#include <framework/backend.h>

namespace Sandbox {

    Camera::Camera(float width, float height) : _isDirty(true),
                                                _eyePosition(glm::vec3(0.0f, 0.0f, 5.0f)),
                                                _lookAtDirection(glm::vec3(0.0f, 0.0f, -1.0f)),
                                                _upVector(glm::vec3(0.0f, 1.0f, 0.0f)),
                                                _width(width),
                                                _height(height),
                                                _fovAngle(75.0f),
                                                _aspectRatio(_width / _height),
                                                _nearPlaneDistance(0.01f),
                                                _farPlaneDistance(1000.0f),
                                                _cameraMatrix(glm::mat4(1.0f)),
                                                _cameraSensitivity(0.1f),
                                                _yaw(-90.0f),
                                                _pitch(0.0f),
                                                _cursorEnabled(true) {
        Backend::Core::SetViewport(0, 0, width, height);
    }

    Camera::~Camera() = default;

    void Camera::SetEyePosition(glm::vec3 position) {
        _eyePosition = position;
        _isDirty = true;
    }

    glm::vec3 &Camera::GetEyePosition() {
        return _eyePosition;
    }

    void Camera::SetLookAtDirection(glm::vec3 direction) {
        _lookAtDirection = direction;
        _isDirty = true;
    }

    glm::vec3 &Camera::GetLookAtDirection() {
        return _lookAtDirection;
    }

    void Camera::SetUpVector(glm::vec3 upVector) {
        _upVector = upVector;
        _isDirty = true;
    }

    glm::vec3 &Camera::GetUpVector() {
        return _upVector;
    }

    const glm::mat4 &Camera::GetMatrix() {
        if (_isDirty) {
            CalculateMatrix();
        }

        return _cameraMatrix;
    }

    void Camera::CalculateMatrix() {
        // Camera matrix
        _viewMatrix = glm::lookAt(_eyePosition, _eyePosition + _lookAtDirection, _upVector);

        // Projection matrix
        _perspectiveMatrix = glm::perspective(glm::radians(_fovAngle), _aspectRatio, _nearPlaneDistance, _farPlaneDistance);

        _cameraMatrix = _perspectiveMatrix * _viewMatrix;
        _isDirty = false;
    }

    const glm::vec3 &Camera::GetForwardVector() const {
        return _lookAtDirection;
    }

    const glm::vec3 &Camera::GetUpVector() const {
        return _upVector;
    }

    void Camera::SetLastMousePosition(glm::vec2 mousePos) {
        _mousePosition = mousePos;
    }

    glm::vec2 &Camera::GetLastMousePosition() {
        return _mousePosition;
    }

    void Camera::SetCameraSensitivity(float sensitivity) {
        _cameraSensitivity = sensitivity;
    }

    float &Camera::GetCameraSensitivity() {
        return _cameraSensitivity;
    }

    void Camera::SetCameraYaw(float yaw) {
        _yaw = yaw;
    }

    float &Camera::GetCameraYaw() {
        return _yaw;
    }

    void Camera::SetCameraPitch(float pitch) {
        _pitch = pitch;
    }

    float &Camera::GetCameraPitch() {
        return _pitch;
    }

    void Camera::SetCursorEnabled(bool enabled) {
        _cursorEnabled = enabled;
    }

    bool &Camera::GetCursorEnabled() {
        return _cursorEnabled;
    }

    const glm::mat4 &Camera::GetPerspectiveMatrix() {
        if (_isDirty) {
            CalculateMatrix();
        }

        return _perspectiveMatrix;
    }

    const glm::mat4 &Camera::GetViewMatrix() {
        if (_isDirty) {
            CalculateMatrix();
        }

        return _viewMatrix;
    }

    float Camera::GetNearPlaneDistance() const {
        return _nearPlaneDistance;
    }

    void Camera::SetNearPlaneDistance(float nearPlaneDistance) {
        _nearPlaneDistance = nearPlaneDistance;
    }

    float Camera::GetFarPlaneDistance() const {
        return _farPlaneDistance;
    }

    void Camera::SetFarPlaneDistance(float farPlaneDistance) {
        _farPlaneDistance = farPlaneDistance;
    }

    float Camera::GetFOV() const {
        return _fovAngle;
    }

    void Camera::SetFOV(float FOV) {
        _fovAngle = FOV;
        _isDirty = true;
    }

    float Camera::GetAspectRatio() const {
        return _aspectRatio;
    }

    void Camera::SetAspectRatio(float aspectRatio) {
        _aspectRatio = aspectRatio;
        _isDirty = true;
    }

}
