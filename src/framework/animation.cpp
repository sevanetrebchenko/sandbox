
#include <framework/animation.h>

namespace Sandbox {

    Animation::Animation() : _duration(0.0f),
                             _speed(0.0f)
                             {
    }

    Animation::~Animation() = default;

    std::pair<KeyPosition, KeyPosition> Animation::GetPositionKeyPair(int boneIndex, float animationTime) const {
        const Track& track = _boneTracks[boneIndex];

        for (unsigned i = 0; i < track._positionKeys.size() - 1; ++i) {
            if (animationTime < track._positionKeys[i + 1].dt) {
                return std::make_pair(track._positionKeys[i], track._positionKeys[i + 1]);
            }
        }

        throw std::runtime_error("Animation time is outside of the animation duration range in GetPositionKeyPair.");
    }

    std::pair<KeyRotation, KeyRotation> Animation::GetRotationKeyPair(int boneIndex, float animationTime) const {
        const Track& track = _boneTracks[boneIndex];

        for (unsigned i = 0; i < track._rotationKeys.size() - 1; ++i) {
            if (animationTime < track._rotationKeys[i + 1].dt) {
                return std::make_pair(track._rotationKeys[i], track._rotationKeys[i + 1]);
            }
        }

        throw std::runtime_error("Animation time is outside of the animation duration range in GetRotationKeyPair.");
    }

    std::pair<KeyScale, KeyScale> Animation::GetScaleKeyPair(int boneIndex, float animationTime) const {
        const Track& track = _boneTracks[boneIndex];

        for (unsigned i = 0; i < track._scaleKeys.size() - 1; ++i) {
            if (animationTime < track._scaleKeys[i + 1].dt) {
                return std::make_pair(track._scaleKeys[i], track._scaleKeys[i + 1]);
            }
        }

        throw std::runtime_error("Animation time is outside of the animation duration range in GetScaleKeyPair.");
    }

}