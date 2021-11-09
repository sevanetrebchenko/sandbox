
#include <framework/transform.h>
#include <glm/gtx/quaternion.hpp>

namespace Sandbox {

    Transform::Transform() : _isDirty(true),
                             _matrix(glm::mat4(1.0f)),
                             _rotation(glm::identity<glm::quat>()),
                             _rotationOffset(glm::identity<glm::quat>()),
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

    glm::mat4 Transform::GetMatrix() const {
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

    void Transform::SetRotation(float degrees, glm::vec3 axis) {
        glm::quat I = glm::identity<glm::quat>();
		_rotation = glm::rotate(I, glm::radians(degrees), axis);
		_isDirty = true;
    }

    glm::quat Transform::GetRotation() const {
        return _rotation;
    }

    void Transform::CalculateMatrix() {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), _position);
        glm::mat4 scale = glm::scale(_scale);

        _matrix = translation * glm::toMat4(_rotationOffset * _rotation) * scale;
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

//        ImGui::Text("Rotation:");
//        if (ImGui::SliderFloat3("##rotation", (float *) (&_rotation), -360.0f, 360.0f)) {
//            _isDirty = true;
//        }
    }

    bool Transform::IsDirty() const {
        return _isDirty;
    }

    void Transform::Clean() {
        if (_isDirty) {
            CalculateMatrix();
        }
    }

	void Transform::SetRotation(glm::quat quaternion) {
		_rotation = quaternion;
		_isDirty = true;
    }

	void Transform::SetRotationOffset(glm::quat rotation) {
		_rotationOffset = rotation;
		_isDirty = true;
	}

}