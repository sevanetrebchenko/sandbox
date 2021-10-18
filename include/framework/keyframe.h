
#ifndef SANDBOX_KEYFRAME_H
#define SANDBOX_KEYFRAME_H

#include <framework/quaternion.h>

namespace Sandbox {

    // Stored animation key types.
    // dt stores the time to which the animation keys need to be interpolated to.
    struct KeyPosition {
        float dt;
        glm::vec3 position;
    };

    struct KeyRotation {
        float dt;
        Quaternion orientation;
    };

    struct KeyScale {
        float dt;
        glm::vec3 scale;
    };

}

#endif //SANDBOX_KEYFRAME_H
