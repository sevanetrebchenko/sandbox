
#include <framework/vqs.h>

namespace Sandbox {

    VQS::VQS() : _v(glm::vec3(0.0f)),
                 _q(), // Default constructor - identity quaternion
                 _s(1.0f)
                 {
    }

    VQS::VQS(const glm::vec3 &translation, const Quaternion &orientation, float scalingFactor) : _v(translation),
                                                                                                 _q(orientation),
                                                                                                 _s(scalingFactor)
                                                                                                 {
    }

    VQS::~VQS() = default;

    const glm::vec3 &VQS::GetTranslation() const {
        return _v;
    }

    const Quaternion &VQS::GetOrientation() const {
        return _q;
    }

    float VQS::GetScalingFactor() const {
        return _s;
    }

    void VQS::SetTranslation(const glm::vec3 &translation) {
        _v = translation;
    }

    void VQS::SetOrientation(const Quaternion &quaternion) {
        _q = quaternion;
    }

    void VQS::SetScalingFactor(float scalingFactor) {
        _s = scalingFactor;
    }

    VQS VQS::operator*(const VQS &other) const {
        // [u, p, t] [v, q, s] = [[u, p, t] * v, pq, st]
        return { _q * other._v + _v, _q * other._q, _s * other._s };
    }

    glm::vec3 VQS::operator*(const glm::vec3 &other) const {
        return _q * other + _v;
    }
}
