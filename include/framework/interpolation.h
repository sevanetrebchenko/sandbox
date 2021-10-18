
#ifndef SANDBOX_INTERPOLATION_H
#define SANDBOX_INTERPOLATION_H

#include <sandbox_pch.h>
#include <framework/quaternion.h>

namespace Sandbox {

    enum class KeyInterpolationMethod {
        DEFAULT,
        INCREMENTAL
    };

    enum class QuaternionInterpolationMethod {
        LERP,
        SLERP,
    };

    [[nodiscard]] std::string ToString(KeyInterpolationMethod method);
    [[nodiscard]] std::string ToString(QuaternionInterpolationMethod method);

    // Linear interpolation.
    [[nodiscard]] Quaternion Lerp(const Quaternion& start, const Quaternion& end, float t);
    [[nodiscard]] Quaternion iLerp(const Quaternion& start, const Quaternion& end, float t, unsigned n);

    // Spherical Linear interpolation.
    [[nodiscard]] Quaternion Slerp(const Quaternion& start, const Quaternion& end, float t);
    [[nodiscard]] Quaternion iSlerp(const Quaternion& start, const Quaternion& end, float t, unsigned n);

}

#endif //SANDBOX_INTERPOLATION_H
