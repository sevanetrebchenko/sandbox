
#include <framework/pather.h>

namespace Sandbox {

    Pather::Pather(VelocityTimeFunction velocityFunction, CenterOfInterestMode centerOfInterestMode)
            : velocityFunction_(velocityFunction),
              centerOfInterestMode_(centerOfInterestMode),
              pathHeight_(1.0f),
              time_(0.0f),
              cycleTime_(10.0f),
              lookAheadDistance_(5.0f)
              {
    }

    Pather::~Pather() {
    }

    void Pather::Update(float dt) {
        if (path_.IsValid()) {
            // Update timestep.
            time_ += dt;
            time_ = std::fmod(time_, cycleTime_);

            switch (velocityFunction_) {
                case VelocityTimeFunction::CONSTANT:
                    ConstantVelocity(dt);
                    break;

                    case VelocityTimeFunction::EASE_IN_OUT:
                        break;

                    case VelocityTimeFunction::USER_DEFINED:
                        break;
            }
        }
    }

    void Pather::ConstantVelocity(float dt) {
    	float arcLength = path_.GetArcLength(1.0f);
    	float velocity = arcLength / cycleTime_;

    	distance_ += velocity * dt;
    	distance_ = std::fmod(distance_, arcLength);

        // Interpolating parameter at the end of the path will always be 1.
        float u = path_.GetInterpolationParameter(distance_);

        // Compute position along the curve.
        glm::dvec2 position = path_.Evaluate(u);
        position_ = glm::vec3(static_cast<float>(position.x), 0.0f, static_cast<float>(position.y));

        // Compute position of focus object.
        float targetDistance = glm::clamp(distance_ + lookAheadDistance_, 0.0f, arcLength);
        float tu = path_.GetInterpolationParameter(targetDistance);

        glm::dvec2 targetPosition = path_.Evaluate(tu);
        lookAt_ = glm::vec3(static_cast<float>(targetPosition.x), 0.0f, static_cast<float>(targetPosition.y));

        // Compute orientation vector.
        glm::dvec2 viewDirection = -glm::normalize(targetPosition - position);
        orientation_ = glm::vec3(static_cast<float>(-viewDirection.y), 0.0f, static_cast<float>(viewDirection.x));
    }

    const glm::vec3 &Pather::GetCurrentPosition() const {
        return position_;
    }

    const glm::vec3 &Pather::GetCurrentPointOfInterest() const {
        return lookAt_;
    }

    const glm::vec3 &Pather::GetCurrentOrientation() const {
        return orientation_;
    }

    float Pather::GetPathHeight() const {
        return pathHeight_;
    }

    Path &Pather::GetPath() {
        return path_;
    }

	float Pather::GetCompletionTime() const {
		return cycleTime_;
	}

	void Pather::SetCompletionTime(float cycleTime) {
		cycleTime_ = cycleTime;
	}

	void Pather::SetLookAheadDistance(float distance) {
		lookAheadDistance_ = distance;
	}

	float Pather::GetLookAheadDistance() const {
		return lookAheadDistance_;
	}

}
