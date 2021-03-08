
#ifndef SANDBOX_LIGHT_H
#define SANDBOX_LIGHT_H

#include <framework/transform.h>

namespace Sandbox {

    class Light {
        public:
            Light();
            explicit Light(const Transform& transform);

            ~Light() = default;

            void SetActive(bool active);
            [[nodiscard]] bool IsActive() const;

            void SetAmbientColor(const glm::vec3& ambientColor);
            [[nodiscard]] const glm::vec3& GetAmbientColor() const;

            void SetDiffuseColor(const glm::vec3& diffuseColor);
            [[nodiscard]] const glm::vec3& GetDiffuseColor() const;

            void SetSpecularColor(const glm::vec3& specularColor);
            [[nodiscard]] const glm::vec3& GetSpecularColor() const;

            [[nodiscard]] bool IsDirty() const;
            [[nodiscard]] Transform& GetTransform();

        private:
            friend class LightingManager;
            void Clean();

            glm::vec3 _ambientColor;
            glm::vec3 _diffuseColor;
            glm::vec3 _specularColor;

            Transform _transform;

            bool _isDirty;
            bool _isActive;
    };

}

#endif //SANDBOX_LIGHT_H
