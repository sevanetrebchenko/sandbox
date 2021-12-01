
#ifndef SANDBOX_PATH_H
#define SANDBOX_PATH_H

#include <sandbox_pch.h>

namespace Sandbox {

    class Path {
        public:
            Path();
            ~Path();

            void AddControlPoint(const glm::dvec2& point);

            [[nodiscard]] bool IsValid() const;

            void Clear();
            void Recompute();

            // p = P(u)
            // Value should be passed in the range [0.0, 1.0].
            [[nodiscard]] glm::dvec2 Evaluate(float u) const;

            // s = G(u)
            // Value should be passed in the range [0.0, 1.0].
            // Returns arc length approximation up until this point.
            [[nodiscard]] float GetArcLength(float u) const;
            [[nodiscard]] float GetGlobalArcLength() const;

            // u = G^-1(s)
            // Returns normalized interpolation parameter based on path arc length s.
            [[nodiscard]] float GetInterpolationParameter(float s) const;

            [[nodiscard]] std::vector<glm::dvec2> GetControlPoints() const;
            void SetControlPoints(const std::vector<glm::dvec2>& controlPoints);

            [[nodiscard]] const std::vector<glm::dvec2>& GetCurveApproximation() const;

        private:
            void Reset();

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

            int curveLOD_; // Total number of points in the curve.
            int maxNumPoints_;
            std::vector<glm::dvec2> curveApproximation_;
            std::vector<glm::vec2> arcLengthTable_; // Pairing: (value of t at this point of the curve, arc length)
            float unnormalized;

            bool isDirty_;
    };

}

#endif //SANDBOX_PATH_H
