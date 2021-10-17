
#ifndef SANDBOX_ANIMATION_H
#define SANDBOX_ANIMATION_H

#include <sandbox_pch.h>
#include <framework/model.h>
#include <framework/bone.h>
#include <framework/track.h>

namespace Sandbox {

    struct Animation {
        Animation();
        ~Animation();

        [[nodiscard]] std::pair<KeyPosition, KeyPosition> GetPositionKeyPair(int boneIndex, float animationTime) const;
        [[nodiscard]] std::pair<KeyRotation, KeyRotation> GetRotationKeyPair(int boneIndex, float animationTime) const;
        [[nodiscard]] std::pair<KeyScale, KeyScale> GetScaleKeyPair(int boneIndex, float animationTime) const;

        std::string _name;
        float _duration;
        float _speed;
        std::vector<Track> _boneTracks;
    };

}

#endif //SANDBOX_ANIMATION_H
