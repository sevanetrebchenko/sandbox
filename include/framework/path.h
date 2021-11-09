
#ifndef SANDBOX_PATH_H
#define SANDBOX_PATH_H

#include <sandbox_pch.h>

namespace Sandbox {

    class Path {
        public:
            Path();
            ~Path();

            void OnImGui();

            void AddControlPoint(const glm::dvec2& point);

            [[nodiscard]] bool IsValid() const;
            void Recompute();

            // Value should be passed in the range [0.0, 1.0].
            [[nodiscard]] glm::dvec2 Evaluate(float t) const;

            // Value should be passed in the range [0.0, 1.0].
            [[nodiscard]] float GetArcLength(float t) const;

            [[nodiscard]] const std::vector<glm::dvec2>& GetControlPoints() const;
            [[nodiscard]] const std::vector<glm::dvec2>& GetCurveApproximation() const;

        private:
            void InterpolatingSplines();
            void ComputeArcLengthTable();

            // Truncated power function (t - c) ^ 3+
            [[nodiscard]] float TruncatedPow3(float t, float c) const;

            // Solve the matrix via Gaussian Elimination.
            [[nodiscard]] std::vector<double> GaussianElimination(std::vector<std::vector<double>> &matrix);

            void ReducedEchelonForm(std::vector<std::vector<double>> &matrix);
            void SwapRow(std::vector<std::vector<double>> &matrix, int i, int j);

            std::vector<glm::dvec2> controlPoints_;
            std::vector<glm::dvec2> polynomial_;

            int curveLOD_; // Number of intermediate curve points between control points.
            int maxNumPoints_;
            std::vector<glm::dvec2> curveApproximation_;
            std::vector<double> curveXCoordinates_;
            std::vector<double> curveYCoordinates_;
            std::vector<glm::vec2> arcLengthTable_; // Pairing: (value of t at this point of the curve, arc length)

            bool isDirty_;
    };

}

#endif //SANDBOX_PATH_H
