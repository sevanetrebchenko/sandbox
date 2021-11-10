
#ifndef SANDBOX_PATHER_H
#define SANDBOX_PATHER_H

#include <framework/path.h>

namespace Sandbox {

    enum class VelocityTimeFunction {
        CONSTANT,
        EASE_IN_OUT,
        USER_DEFINED
    };

    [[nodiscard]] std::string ToString(VelocityTimeFunction function);

    enum class CenterOfInterestMode {
        ORBIT,
        FORWARD
    };

    [[nodiscard]] std::string ToString(CenterOfInterestMode mode);

    class Pather {
        public:
            explicit Pather(VelocityTimeFunction velocityFunction = VelocityTimeFunction::CONSTANT,
                            CenterOfInterestMode centerOfInterestMode = CenterOfInterestMode::FORWARD);
            ~Pather();

            void Update(float dt);

            void SetCompletionTime(float cycleTime);
            void SetLookAheadDistance(int distance);
            void SetOrbitFocus(glm::dvec2 orbitFocus);
            void SetCenterOfInterestMode(CenterOfInterestMode mode);
            void SetVelocityTimeFunction(VelocityTimeFunction function);
            void SetT1(float t1);
            void SetT2(float t2);

            [[nodiscard]] const glm::vec3& GetCurrentPosition() const;
            [[nodiscard]] const glm::vec3& GetCurrentPointOfInterest() const;
            [[nodiscard]] const glm::vec3& GetCurrentOrientation() const;

            [[nodiscard]] float GetPathHeight() const;
            [[nodiscard]] Path& GetPath();
            [[nodiscard]] float GetCompletionTime() const;
            [[nodiscard]] int GetLookAheadDistance() const;
            [[nodiscard]] int GetMaxLookingDistance() const;
            [[nodiscard]] glm::dvec2 GetOrbitFocus() const;
            [[nodiscard]] CenterOfInterestMode GetCenterOfInterestMode() const;
            [[nodiscard]] VelocityTimeFunction GetVelocityTimeFunction() const;

            [[nodiscard]] float GetCurrentArcLength() const;
            [[nodiscard]] float GetCurrentInterpolationParameter() const;
            [[nodiscard]] float GetT1() const;
            [[nodiscard]] float GetT2() const;

        private:
            Path path_;
            VelocityTimeFunction velocityFunction_;
            CenterOfInterestMode centerOfInterestMode_;

            glm::vec3 lookAt_;
            glm::vec3 position_;
            glm::vec3 orientation_;

            float pathHeight_;
            float distance_;
            float u_;

            // Focus point for Orbit Center of Interest mode.
            glm::dvec2 orbitFocus_;

            // Forward.
            int lookAheadDistance_;
            int maxLookDistance_;

            // Parabolic.
            float t1_;
            float t2_;

            float time_;
            float cycleTime_;
    };

}

#endif //SANDBOX_PATHER_H
