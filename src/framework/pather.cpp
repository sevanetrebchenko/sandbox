
#include <framework/pather.h>

namespace Sandbox {

    Pather::Pather(VelocityTimeFunction velocityFunction, CenterOfInterestMode centerOfInterestMode)
            : velocityFunction_(velocityFunction),
              centerOfInterestMode_(centerOfInterestMode),
              pathHeight_(1.0f),
              time_(0.0f),
              cycleTime_(10.0f)
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
        float focusTime = glm::clamp(time_ + 0.1f, 0.0f, cycleTime_) / cycleTime_;
        glm::dvec2 focusPosition = path_.Evaluate(focusTime);
        lookAt_ = glm::vec3(static_cast<float>(focusPosition.x), 0.0f, static_cast<float>(focusPosition.y));

        // Compute final orientation.
        glm::vec2 w = glm::normalize(focusPosition - position);
        glm::vec3 W = glm::vec3(w.x, 0.0f, w.y);
        glm::vec3 U = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), W);
        glm::vec3 V = glm::cross(W, U);

        orientation_ = V;
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

}
