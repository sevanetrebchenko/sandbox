
#include <framework/light.h>

namespace Sandbox {

    Light::Light() : _ambientColor(glm::vec3(1.0f)),
                     _diffuseColor(glm::vec3(1.0f)),
                     _specularColor(glm::vec3(1.0f)),
                     _isDirty(true),
                     _isActive(true)
    {
        _transform.SetScale(glm::vec3(0.1f));
    }

    Light::Light(const Transform &transform) : _transform(transform),
                                               _ambientColor(glm::vec3(1.0f)),
                                               _diffuseColor(glm::vec3(1.0f)),
                                               _specularColor(glm::vec3(1.0f)),
                                               _isDirty(true),
                                               _isActive(true)
    {
    }

    void Light::SetAmbientColor(const glm::vec3 &ambientColor) {
        _ambientColor = ambientColor;
        _isDirty = true;
    }

    const glm::vec3 &Light::GetAmbientColor() const {
        return _ambientColor;
    }

    void Light::SetDiffuseColor(const glm::vec3 &diffuseColor) {
        _diffuseColor = diffuseColor;
        _isDirty = true;
    }

    const glm::vec3 &Light::GetDiffuseColor() const {
        return _diffuseColor;
    }

    void Light::SetSpecularColor(const glm::vec3 &specularColor) {
        _specularColor = specularColor;
        _isDirty = true;
    }

    const glm::vec3 &Light::GetSpecularColor() const {
        return _specularColor;
    }

    bool Light::IsDirty() const {
        return _isDirty || _transform.IsDirty();
    }

    void Light::Clean() {
        _isDirty = false;
    }

    void Light::SetActive(bool active) {
        _isActive = active;
        _isDirty = true;
    }

    bool Light::IsActive() const {
        return _isActive;
    }

    Transform &Light::GetTransform() {
        return _transform;
    }

}
