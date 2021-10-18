
#include <framework/interpolation.h>

namespace Sandbox {

    std::string ToString(KeyInterpolationMethod method) {
        switch (method) {
            case KeyInterpolationMethod::DEFAULT:
                return "Default";
            case KeyInterpolationMethod::INCREMENTAL:
                return "Incremental";
            default:
                throw std::runtime_error("Unknown KeyInterpolationMethod provided to ToString()");
        }
    }

    std::string ToString(QuaternionInterpolationMethod method) {
        switch (method) {
            case QuaternionInterpolationMethod::LERP:
                return "Lerp";
            case QuaternionInterpolationMethod::SLERP:
                return "Slerp";
            default:
                throw std::runtime_error("Unknown QuaternionInterpolationMethod provided to ToString()");
        }
    }

    Quaternion Lerp(const Quaternion &start, const Quaternion &end, float t) {
        // Use unit quaternions to avoid the magnitude effect.
        Quaternion q0 = Quaternion::Normalize(start);
        Quaternion q1 = Quaternion::Normalize(end);

        float cosTheta = Quaternion::DotProduct(q0, q1);

        // Always take the smallest path to interpolate over.
        if (cosTheta < 0.0f) {
            q1 *= -1.0f;
        }

        // Lerp does not guarantee a unit quaternion result.
        return Quaternion::Normalize((1.0f - t) * q0 + t * q1);
    }

    Quaternion iLerp(const Quaternion &start, const Quaternion &end, float t, unsigned n) {
        float cosTheta = Quaternion::DotProduct(start, end);

        // Use unit quaternions to avoid the magnitude effect.
        Quaternion q0 = Quaternion::Normalize(start);
        Quaternion q1 = Quaternion::Normalize(end);

        // Always take the smallest path to interpolate over.
        if (cosTheta < 0.0f) {
            q1 *= -1.0f;
        }

        Quaternion step = (q1 - q0) * (1.0f / static_cast<float>(n));
        float k = t * static_cast<float>(n);

        // iLerp does not guarantee a unit quaternion.
        return Quaternion::Normalize(start + step * k);
    }

    Quaternion Slerp(const Quaternion &start, const Quaternion &end, float t) {
        // Slerp gives back unit quaternion if start / end are unit quaternions.
        Quaternion q0 = Quaternion::Normalize(start);
        Quaternion q1 = Quaternion::Normalize(end);

        float cosTheta = Quaternion::DotProduct(start, end);

        // Always take the smallest path to interpolate over.
        if (cosTheta < 0.0f) {
            q1 *= -1.0f;
            cosTheta *= -1.0f; // cos of angle also depends on q1 direction.
        }

        // Linearly interpolate if too close together.
        // Dot product of to near-identical vectors is 1.0f.
        if (cosTheta > (1.0f - std::numeric_limits<float>::epsilon())) {
            return Lerp(start, end, t); // Already normalized.
        }

        float angle = std::acos(cosTheta);

        float sata = std::sin(angle - t * angle);
        float sta = std::sin(t * angle);
        float sa = std::sin(angle);

        return (sata / sa) * q0 + (sta / sa) * q1;
    }

    Quaternion iSlerp(const Quaternion &start, const Quaternion &end, float t, unsigned n) {
        // Slerp gives back unit quaternion if start / end are unit quaternions.
        Quaternion q0 = Quaternion::Normalize(start);
        Quaternion qn = Quaternion::Normalize(end);

        float cosTheta = Quaternion::DotProduct(start, end);

        // Always take the smallest path to interpolate over.
        if (cosTheta < 0.0f) {
            qn *= -1.0f;
            cosTheta *= -1.0f; // cos of angle also depends on q1 direction.
        }

        // Linearly interpolate if too close together.
        // Dot product of to near-identical vectors is 1.0f.
        if (cosTheta > (1.0f - std::numeric_limits<float>::epsilon())) {
            return iLerp(start, end, t, n); // Already normalized.
        }

        float alpha = std::acos(cosTheta);
        float beta = alpha / static_cast<float>(n);

        float iterationMultiplier = 2.0f * std::cos(beta);

        // First iteration, k = 1.
        // qPrevious = q(k - 1)
        // qPrevious = q(0)
        Quaternion qPrevious = q0;

        // qn = q(k)

        // qCurrent = q(k + 1)
        // qCurrent = q(2)
        Quaternion qCurrent = (std::sin(alpha - 2.0f * beta) * q0 + std::sin(2.0f * beta) * qn) / std::sin(alpha);

        for (unsigned k = 2; k < static_cast<unsigned>(glm::floor(t * static_cast<float>(n))); ++k) {
            Quaternion previous = qCurrent;
            qCurrent = iterationMultiplier * qCurrent - qPrevious; // Compute next iteration.
            qPrevious = previous;
        }

        return qCurrent;
    }

}
