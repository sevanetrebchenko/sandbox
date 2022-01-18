
#include <framework/camera.h>
#include <framework/backend.h>

namespace Sandbox {

    ICamera::ICamera(float width, float height) : _isDirty(true),
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

    ICamera::~ICamera() = default;

    void ICamera::SetEyePosition(glm::vec3 position) {
        _eyePosition = position;
        _isDirty = true;
    }

    glm::vec3 &ICamera::GetEyePosition() {
        return _eyePosition;
    }

    void ICamera::SetLookAtDirection(glm::vec3 direction) {
        _lookAtDirection = direction;
        _isDirty = true;
    }

    glm::vec3 &ICamera::GetLookAtDirection() {
        return _lookAtDirection;
    }

    void ICamera::SetUpVector(glm::vec3 upVector) {
        _upVector = upVector;
        _isDirty = true;
    }

    glm::vec3 &ICamera::GetUpVector() {
        return _upVector;
    }

    const glm::mat4 &ICamera::GetMatrix() {
        if (_isDirty) {
            CalculateMatrix();
        }

        return _cameraMatrix;
    }

    void ICamera::CalculateMatrix() {
        // Camera matrix
        _viewMatrix = glm::lookAt(_eyePosition, _eyePosition + _lookAtDirection, _upVector);

        // Projection matrix
        _perspectiveMatrix = glm::perspective(glm::radians(_fovAngle), _aspectRatio, _nearPlaneDistance, _farPlaneDistance);

        _cameraMatrix = _perspectiveMatrix * _viewMatrix;
        _isDirty = false;
    }

    const glm::vec3 &ICamera::GetForwardVector() const {
        return _lookAtDirection;
    }

    const glm::vec3 &ICamera::GetUpVector() const {
        return _upVector;
    }

    void ICamera::SetLastMousePosition(glm::vec2 mousePos) {
        _mousePosition = mousePos;
    }

    glm::vec2 &ICamera::GetLastMousePosition() {
        return _mousePosition;
    }

    void ICamera::SetCameraSensitivity(float sensitivity) {
        _cameraSensitivity = sensitivity;
    }

    float &ICamera::GetCameraSensitivity() {
        return _cameraSensitivity;
    }

    void ICamera::SetCameraYaw(float yaw) {
        _yaw = yaw;
    }

    float &ICamera::GetCameraYaw() {
        return _yaw;
    }

    void ICamera::SetCameraPitch(float pitch) {
        _pitch = pitch;
    }

    float &ICamera::GetCameraPitch() {
        return _pitch;
    }

    void ICamera::SetCursorEnabled(bool enabled) {
        _cursorEnabled = enabled;
    }

    bool &ICamera::GetCursorEnabled() {
        return _cursorEnabled;
    }

    const glm::mat4 &ICamera::GetPerspectiveMatrix() {
        if (_isDirty) {
            CalculateMatrix();
        }

        return _perspectiveMatrix;
    }

    const glm::mat4 &ICamera::GetViewMatrix() {
        if (_isDirty) {
            CalculateMatrix();
        }

        return _viewMatrix;
    }

    float ICamera::GetNearPlaneDistance() const {
        return _nearPlaneDistance;
    }

    void ICamera::SetNearPlaneDistance(float nearPlaneDistance) {
        _nearPlaneDistance = nearPlaneDistance;
    }

    float ICamera::GetFarPlaneDistance() const {
        return _farPlaneDistance;
    }

    void ICamera::SetFarPlaneDistance(float farPlaneDistance) {
        _farPlaneDistance = farPlaneDistance;
    }

    float ICamera::GetFOV() const {
        return _fovAngle;
    }

    void ICamera::SetFOV(float FOV) {
        _fovAngle = FOV;
        _isDirty = true;
    }

    float ICamera::GetAspectRatio() const {
        return _aspectRatio;
    }

    void ICamera::SetAspectRatio(float aspectRatio) {
        _aspectRatio = aspectRatio;
        _isDirty = true;
    }

}
