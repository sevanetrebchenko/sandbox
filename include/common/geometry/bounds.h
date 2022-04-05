
#pragma once

namespace Sandbox {

    class Bounds {
        public:
            Bounds();
            Bounds(const glm::vec3& point);
            Bounds(const glm::vec3& minimum, const glm::vec3& maximum);
            ~Bounds();

            void Extend(const glm::vec3& point);
            void Extend(float scale);

            [[nodiscard]] glm::vec3 GetCentroid() const;

            [[nodiscard]] const glm::vec3& GetMinimum() const;
            [[nodiscard]] const glm::vec3& GetMaximum() const;

            [[nodiscard]] glm::vec3 GetDiagonal() const;
            [[nodiscard]] float GetSurfaceArea() const;
            [[nodiscard]] float GetVolume() const;
            [[nodiscard]] int GetMaximumAxis() const;

            [[nodiscard]] bool Inside(const glm::vec3& point) const;

            [[nodiscard]] static Bounds GetUnion(const Bounds& first, const Bounds& second);
            [[nodiscard]] static Bounds GetIntersection(const Bounds& first, const Bounds& second);
            [[nodiscard]] static bool Overlap(const Bounds& first, const Bounds& second);

        private:
            bool initialized_; // Default-constructed bounds should not include glm::vec3(0.0f) in the bounds (unless explicitly specified).
            glm::vec3 minimum_;
            glm::vec3 maximum_;
    };

}