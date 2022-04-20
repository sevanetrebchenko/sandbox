
#pragma once

#include "common/ecs/component/component.h"

namespace Sandbox {

    struct Refractive : public IComponent {
        float ior;
    };

}