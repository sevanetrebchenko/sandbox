
#include <framework/pather.h>

namespace Sandbox {

	Pather::Pather(VelocityTimeFunction velocityFunction, CenterOfInterestMode centerOfInterestMode)
		: velocityFunction_(velocityFunction),
		  centerOfInterestMode_(centerOfInterestMode),
		  pathHeight_(1.0f),
		  distance_(0.0f),
		  u_(0.0f),
		  time_(0.0f),
		  cycleTime_(10.0f),
		  lookAheadDistance_(5),
		  maxLookDistance_(20),
		  orbitFocus_(glm::dvec2(0.0f)),
		  t1_(0.3f),
		  t2_(0.7f)
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
					break;
				}
				case VelocityTimeFunction::EASE_IN_OUT: {
					// Interpolant.
					float t = time_ / cycleTime_;

					// Final constant velocity.
					float v0 = 2.0f / (1.0f - t1_ + t2_);

					if (t >= 0.0f && t <= t1_) {
						distance_ = (v0 / (2.0f * t1_)) * (t * t);
					}
					else if (t > t1_ && t < t2_) {
						distance_ = v0 * (t - (t1_ / 2.0f));
					}
					else if (t >= t2_ && t <= 1.0f) {
						distance_ = ((v0 * (t - t2_)) / (2.0f * (1.0f - t2_))) * (2.0f - t - t2_) + v0 * (t2_ - (t1_ / 2.0f));
					}

					break;
				}

				case VelocityTimeFunction::USER_DEFINED:
					// Not implemented.
					break;
			}

			if (distance_ > arcLength - (0.5f / path_.GetGlobalArcLength())) {

				distance_ = std::clamp(distance_, 0.0f, arcLength - (0.5f / path_.GetGlobalArcLength()));
			}

			// Interpolating parameter at the end of the path will always be 1.
			u_ = path_.GetInterpolationParameter(distance_);

			// Compute position along the curve.
			glm::dvec2 position = path_.Evaluate(u_);
			position_ = glm::vec3(static_cast<float>(position.x), 0.0f, static_cast<float>(position.y));


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
			orientation_ = glm::vec3(static_cast<float>(-viewDirection.y), 0.0f, static_cast<float>(viewDirection.x));
		}
	}

	const glm::vec3& Pather::GetCurrentPosition() const {
		return position_;
	}

	const glm::vec3& Pather::GetCurrentPointOfInterest() const {
		return lookAt_;
	}

	const glm::vec3& Pather::GetCurrentOrientation() const {
		return orientation_;
	}

	float Pather::GetPathHeight() const {
		return pathHeight_;
	}

	Path& Pather::GetPath() {
		return path_;
	}

	float Pather::GetCompletionTime() const {
		return cycleTime_;
	}

	void Pather::SetCompletionTime(float cycleTime) {
		cycleTime_ = cycleTime;
		time_ = 0.0f;
		distance_ = 0.0f;
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

	glm::dvec2 Pather::GetOrbitFocus() const {
		return orbitFocus_;
	}

	void Pather::SetCenterOfInterestMode(CenterOfInterestMode mode) {
		centerOfInterestMode_ = mode;
		time_ = 0.0f;
		distance_ = 0.0f;
	}

	void Pather::SetVelocityTimeFunction(VelocityTimeFunction function) {
		velocityFunction_ = function;
		time_ = 0.0f;
		distance_ = 0.0f;
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

	float Pather::GetCurrentArcLength() const {
		return distance_;
	}

	float Pather::GetCurrentInterpolationParameter() const {
		return u_;
	}

	float Pather::GetT1() const {
		return t1_;
	}

	float Pather::GetT2() const {
		return t2_;
	}

	void Pather::SetT1(float t1) {
		t1_ = t1;
		time_ = 0.0f;
		distance_ = 0.0f;
	}

	void Pather::SetT2(float t2) {
		t2_ = t2;
		time_ = 0.0f;
		distance_ = 0.0f;
	}

    void Pather::Reset() {
        distance_ = 0;
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
