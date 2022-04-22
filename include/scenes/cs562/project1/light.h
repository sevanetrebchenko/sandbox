
#pragma once

#include "pch.h"
#include "common/ecs/component/component.h"

namespace Sandbox {

    struct DirectionalLight {
        DirectionalLight();
        DirectionalLight(const glm::vec3& direction, const glm::vec3& color, float brightness);

        glm::vec3 direction_;
        glm::vec3 color_;
        glm::vec3 position_;
        float brightness_;
    };

    struct LocalLight : public IComponent {
        LocalLight();
        LocalLight(const glm::vec3& color, float brightness);
        ~LocalLight() override = default;

        glm::vec3 color_;
        float brightness_;
    };

}
