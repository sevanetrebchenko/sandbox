
#include <framework/pather.h>

namespace Sandbox {

    Pather::Pather(VelocityTimeFunction velocityFunction, CenterOfInterestMode centerOfInterestMode)
            : velocityFunction_(velocityFunction),
              centerOfInterestMode_(centerOfInterestMode),
              pathHeight_(1.0f),
              time_(0.0f),
              cycleTime_(10.0f),
              lookAheadDistance_(5),
              maxLookDistance_(20),
              orbitFocus_(glm::vec2(0.0f))
              {
    }

    Pather::~Pather() {
    }

    void Pather::Update(float dt) {
        if (path_.IsValid()) {
            // Update timestep.
            time_ += dt;
            time_ = std::fmod(time_, cycleTime_);

            float arcLength = path_.GetArcLength(1.0f);

            switch (velocityFunction_) {
                case VelocityTimeFunction::CONSTANT: {
                	float velocity = arcLength / cycleTime_;

                	distance_ += velocity * dt;
                	distance_ = std::fmod(distance_, arcLength);

                	// Interpolating parameter at the end of the path will always be 1.
                	float u = path_.GetInterpolationParameter(distance_);

                	// Compute position along the curve.
                	glm::dvec2 position = path_.Evaluate(u);
                	position_ = glm::vec3(static_cast<float>(position.x), 0.0f, static_cast<float>(position.y));

                	break;
                }
				case VelocityTimeFunction::EASE_IN_OUT:
					break;

				case VelocityTimeFunction::USER_DEFINED:
					break;
            }

            glm::dvec2 targetPosition;

            switch (centerOfInterestMode_) {
				case CenterOfInterestMode::ORBIT: {
					targetPosition = orbitFocus_;
					break;
				}

				case CenterOfInterestMode::FORWARD: {
					// Compute position of focus object.
					float step = 1.0f / static_cast<float>(maxLookDistance_);

					float targetDistance = glm::clamp(distance_ + (static_cast<float>(lookAheadDistance_) * step), 0.0f, arcLength);
					float tu = path_.GetInterpolationParameter(targetDistance);
					targetPosition = path_.Evaluate(tu);
					break;
				}
			}

			lookAt_ = glm::vec3(static_cast<float>(targetPosition.x), 0.0f, static_cast<float>(targetPosition.y));

			// Compute orientation vector.
			glm::dvec2 viewDirection = -glm::normalize(targetPosition - glm::dvec2(position_.x, position_.z));
            orientation_ = glm::vec3(static_cast<float>(-viewDirection.y), 0.0f, static_cast<float>(viewDirection.x
            ));
        }
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

	void Pather::SetLookAheadDistance(int distance) {
		lookAheadDistance_ = distance;
	}

	int Pather::GetLookAheadDistance() const {
		return lookAheadDistance_;
	}

	void Pather::SetOrbitFocus(glm::dvec2 orbitFocus) {
		orbitFocus_ = orbitFocus;
	}

	glm::vec2 Pather::GetOrbitFocus() const {
		return orbitFocus_;
	}

	void Pather::SetCenterOfInterestMode(CenterOfInterestMode mode) {
		centerOfInterestMode_ = mode;
	}

	void Pather::SetVelocityTimeFunction(VelocityTimeFunction function) {
		velocityFunction_ = function;
	}

	CenterOfInterestMode Pather::GetCenterOfInterestMode() const {
		return centerOfInterestMode_;
	}

	VelocityTimeFunction Pather::GetVelocityTimeFunction() const {
		return velocityFunction_;
	}

	int Pather::GetMaxLookingDistance() const {
		return maxLookDistance_;
	}

	std::string ToString(VelocityTimeFunction function) {
    	switch (function) {
			case VelocityTimeFunction::CONSTANT:
				return "Constant";
			case VelocityTimeFunction::EASE_IN_OUT:
				return "Ease In / Ease Out";
			case VelocityTimeFunction::USER_DEFINED:
				return "User Defined";
			}

		return "";
	}

	std::string ToString(CenterOfInterestMode mode) {
		switch (mode) {
			case CenterOfInterestMode::ORBIT:
				return "Orbit";
			case CenterOfInterestMode::FORWARD:
				return "Forward";
		}

		return "";
	}
}
