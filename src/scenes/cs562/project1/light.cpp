
#include "scenes/cs562/project1/light.h"

namespace Sandbox {

    DirectionalLight::DirectionalLight() : direction_(glm::vec3(0.0f, -1.0f, 0.0f)),
                                           color_(glm::vec3(1.0f)),
                                           brightness_(1.0f)
                                           {
    }

    DirectionalLight::DirectionalLight(const glm::vec3& direction, const glm::vec3& color, float brightness) : direction_(direction),
                                                                                                               color_(color),
                                                                                                               brightness_(brightness) {
    }

    LocalLight::LocalLight() : color_(glm::vec3(1.0f)),
                               brightness_(1.0f)
                               {
    }

    LocalLight::LocalLight(const glm::vec3& color, float brightness) : color_(color),
                                                                       brightness_(brightness)
                                                                       {
    }

}
