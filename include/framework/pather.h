
#ifndef SANDBOX_PATHER_H
#define SANDBOX_PATHER_H

#include <framework/path.h>

namespace Sandbox {

    enum class VelocityTimeFunction {
        CONSTANT,
        EASE_IN_OUT,
        USER_DEFINED
    };

    enum class CenterOfInterestMode {
        ORBIT,
        FORWARD,
        TETHER,
        COMBINED
    };

    class Pather {
        public:
            explicit Pather(VelocityTimeFunction velocityFunction = VelocityTimeFunction::CONSTANT,
                            CenterOfInterestMode centerOfInterestMode = CenterOfInterestMode::FORWARD);
            ~Pather();

            void Update(float dt);

            void SetCompletionTime(float cycleTime);
            void SetLookAheadDistance(float distance);

            [[nodiscard]] const glm::vec3& GetCurrentPosition() const;
            [[nodiscard]] const glm::vec3& GetCurrentPointOfInterest() const;
            [[nodiscard]] const glm::vec3& GetCurrentOrientation() const;

            [[nodiscard]] float GetPathHeight() const;
            [[nodiscard]] Path& GetPath();
            [[nodiscard]] float GetCompletionTime() const;
            [[nodiscard]] float GetLookAheadDistance() const;

        private:
            void ConstantVelocity(float dt);

            Path path_;
            VelocityTimeFunction velocityFunction_;
            CenterOfInterestMode centerOfInterestMode_;

            glm::vec3 lookAt_;
            glm::vec3 position_;
            glm::vec3 orientation_;

            float pathHeight_;
            float distance_;
            float lookAheadDistance_;

            float time_;
            float cycleTime_;
    };

}

#endif //SANDBOX_PATHER_H
