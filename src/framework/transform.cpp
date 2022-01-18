
#include <framework/transform.h>

namespace Sandbox {

    Transform::Transform() : _isDirty(true),
                             _matrix(glm::mat4(1.0f)),
                             _rotation(glm::vec3(0.0f)),
                             _scale(glm::vec3(1.0f)),
                             _position(glm::vec3(0.0f)) {
        CalculateMatrix();
    }

    Transform::~Transform() = default;

    void Transform::SetPosition(glm::vec3 position) {
        if (_position != position) {
            _position = position;
            _isDirty = true;
        }
    }

    glm::vec3 Transform::GetPosition() const {
        return _position;
    }

    glm::mat4 Transform::GetMatrix() {
        if (_isDirty) {
            CalculateMatrix();
        }

        return _matrix;
    }

    void Transform::SetScale(glm::vec3 scale) {
        if (_scale != scale) {
            _scale = scale;
            _isDirty = true;
        }
    }

    glm::vec3 Transform::GetScale() const {
        return _scale;
    }

    void Transform::SetRotation(glm::vec3 rotation) {
        if (rotation.x > 360 || rotation.x < -360) {
            rotation.x = 0;
        }

        if (rotation.y > 360 || rotation.y < -360) {
            rotation.y = 0;
        }

        if (rotation.z > 360 || rotation.z < -360) {
            rotation.z = 0;
        }

        if (_rotation != rotation) {
            _rotation = rotation;
            _isDirty = true;
        }
    }

    glm::vec3 Transform::GetRotation() const {
        return _rotation;
    }

    void Transform::CalculateMatrix() {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), _position);
        glm::mat4 scale = glm::scale(_scale);
        glm::mat4 rotationX = glm::rotate(glm::radians(_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotationY = glm::rotate(glm::radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotationZ = glm::rotate(glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        _matrix = translation * rotationX * rotationY * rotationZ * scale;
        _isDirty = false;
    }

    void Transform::OnImGui() {
        ImGui::Text("Position:");
        if (ImGui::SliderFloat3("##position", (float *) (&_position), -5.0f, 5.0f)) {
            _isDirty = true;
        }

        ImGui::Text("Scale:");
        if (ImGui::SliderFloat3("##scale", (float *) (&_scale), -5.0f, 5.0f)) {
            _isDirty = true;
        }

        ImGui::Text("Rotation:");
        if (ImGui::SliderFloat3("##rotation", (float *) (&_rotation), -360.0f, 360.0f)) {
            _isDirty = true;
        }
    }

    bool Transform::IsDirty() const {
        return _isDirty;
    }

}