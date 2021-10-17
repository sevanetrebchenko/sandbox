
#ifndef SANDBOX_TRACK_H
#define SANDBOX_TRACK_H

#include <sandbox_pch.h>
#include <framework/keyframe.h>

namespace Sandbox {

    struct Track {
        std::string _name;

        // Key data for this track.
        std::vector<KeyPosition> _positionKeys;
        std::vector<KeyRotation> _rotationKeys;
        std::vector<KeyScale> _scaleKeys;
    };

}

#endif //SANDBOX_TRACK_H
