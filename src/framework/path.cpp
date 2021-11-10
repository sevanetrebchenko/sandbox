
#include <framework/path.h>
#include <implot.h>

namespace Sandbox {

	Path::Path() : curveLOD_(200),
				   maxNumPoints_(20),
				   isDirty_(false) {
	}

	Path::~Path() {
	}

	void Path::AddControlPoint(const glm::dvec2& point) {
		if (controlPoints_.size() < maxNumPoints_) {
			controlPoints_.push_back(point);
			isDirty_ = true;

			Recompute();
		}
	}

	void Path::Recompute() {
		if (isDirty_) {
			Reset();

			// Recompute curve.
			InterpolatingSplines();

			// Compute arc length table.
			ComputeArcLengthTable();

//            float steps = 1000.0f;
//            float step = 1.0f / steps;
//
//            for (int i = 0; i < (int)steps; ++i) {
//                float s = GetArcLength((float)i * step);
//                float u = GetInterpolationParameter(s);
//
//                std::cout << "at arc length: " << s << ", interpolation parameter: " << u << std::endl;
//            }
//
//            std::cout << std::endl;
//            std::cout << std::endl;
//
			isDirty_ = false;
		}
	}

	glm::dvec2 Path::Evaluate(float u) const {
		u *= (controlPoints_.size() - 1);

		glm::dvec2 value(0.0f);

		// a0 = 1
		value.x += polynomial_[0].x;
		value.y += polynomial_[0].y;

		// a1 = u
		value.x += polynomial_[1].x * u;
		value.y += polynomial_[1].y * u;

		// a2 = u ^ 2
		value.x += polynomial_[2].x * u * u;
		value.y += polynomial_[2].y * u * u;

		// a3 = u ^ 3
		value.x += polynomial_[3].x * u * u * u;
		value.y += polynomial_[3].y * u * u * u;

		// Truncated power functions.
		for (int c = 1; c <= polynomial_.size() - 4; ++c) {
			float multiplier = TruncatedPow3(u, static_cast<float>(c));
			value.x += polynomial_[3 + c].x * multiplier;
			value.y += polynomial_[3 + c].y * multiplier;
		}

		return value;
	}

	std::vector<glm::dvec2> Path::GetControlPoints() const {
		return controlPoints_;
	}

	const std::vector<glm::dvec2>& Path::GetCurveApproximation() const {
		return curveApproximation_;
	}

	void Path::InterpolatingSplines() {
		int numPoints = controlPoints_.size();

		// No curve with 1 point.
		if (numPoints <= 1) {
			return;
		}

		int k = numPoints - 1; // 0, 1, 2, 3, 4, .... k (k will be n - 1)
		int numCoefficients = numPoints + 2; // numPoints + f''(0) and f''(k)

		std::vector<std::vector<glm::dvec2>> linearSystem;
		linearSystem.resize(numCoefficients);

		// Preallocate.
		for (std::vector<glm::dvec2>& set : linearSystem) {
			set.resize(numCoefficients + 1);
		}

		// Initialize points inside linear system.
		for (int i = 0; i < numCoefficients; ++i) {
			for (int j = 0; j < numCoefficients + 1; ++j) {
				linearSystem[i][j] = glm::dvec2(0.0f);
			}
		}

		for (int i = 0; i < numPoints; ++i) {
			// From 0 -> 1 is just a regular cubic function.
			linearSystem[i][0] = glm::dvec2(1, 1);                 // a0 -> 1
			linearSystem[i][1] = glm::dvec2(i, i);                 // a1 -> t
			linearSystem[i][2] = glm::dvec2(i * i, i * i);         // a2 -> t^2
			linearSystem[i][3] = glm::dvec2(i * i * i, i * i * i); // a3 -> t^3

			// Shifted power basis from c = 1 -> k - 1
			for (int c = 1; c <= k - 1; ++c) {
				float value = TruncatedPow3(static_cast<float>(i), static_cast<float>(c));
				linearSystem[i][3 + c] = glm::dvec2(value, value);
			}

			// Emplace data point.
			linearSystem[i][numCoefficients] = controlPoints_[i];
		}

		// Emplace f''(0) = 0 and f''(k) = 0 as the last 2 equations.
		// f''(0) = 0
		linearSystem[numPoints][0] = glm::dvec2(0, 0); // a0       -> 0
		linearSystem[numPoints][1] = glm::dvec2(0, 0); // a1 t     -> 0
		linearSystem[numPoints][2] = glm::dvec2(2, 2); // a2 t ^ 2 -> 2

		for (int i = 3; i <= numPoints + 1; ++i) {
			linearSystem[numPoints][i] = glm::dvec2(0, 0); // All other variables are 0.
		}

		// f''(k) = 0
		linearSystem[numPoints + 1][0] = glm::dvec2(0, 0); // a0        -> 0
		linearSystem[numPoints + 1][1] = glm::dvec2(0, 0); // a1 t      -> 0
		linearSystem[numPoints + 1][2] = glm::dvec2(2, 2); // a2 t ^ 2  -> 2
		linearSystem[numPoints + 1][3] = glm::dvec2(6 * k, 6 * k); // a3 t ^ 3  ->  6t

		for (int i = 1; i <= k - 1; ++i) {
			if (k - i < 0) {
				linearSystem[numPoints + 1][3 + i] = glm::dvec2(0, 0);
			}
			else {
				linearSystem[numPoints + 1][3 + i] = glm::dvec2(6 * (k - i), 6 * (k - i));
			}
		}

		// Emplace final 0.
		linearSystem[numPoints + 1][numCoefficients] = glm::dvec2(0, 0);

		// Solve system using Gaussian elimination.
		std::vector<std::vector<double>> xCoefficients{};
		xCoefficients.resize(numCoefficients);
		for (std::vector<double>& set : xCoefficients) {
			set.resize(numCoefficients + 1);
		}

		std::vector<std::vector<double>> yCoefficients{};
		yCoefficients.resize(numCoefficients);
		for (std::vector<double>& set : yCoefficients) {
			set.resize(numCoefficients + 1);
		}

		// Emplace initial data.
		for (int i = 0; i < numCoefficients; ++i) {
			for (int j = 0; j < numCoefficients + 1; ++j) {
				xCoefficients[i][j] = linearSystem[i][j].x;
				yCoefficients[i][j] = linearSystem[i][j].y;
			}
		}

		std::vector<double> xSolution = GaussianElimination(xCoefficients);
		std::vector<double> ySolution = GaussianElimination(yCoefficients);

		polynomial_.resize(numCoefficients);

		// Emplace solution.
		for (int i = 0; i < numCoefficients; ++i) {
			polynomial_[i] = glm::dvec2(xSolution[i], ySolution[i]);
		}

		// Calculate curve.
		float alpha = 1.0f / static_cast<float>(curveLOD_);

		for (int i = 0; i < curveLOD_; ++i) {
			curveApproximation_.push_back(Evaluate(alpha * static_cast<float>(i)));
		}
	}

	// Truncated power function (t - c) ^ 3+
	float Path::TruncatedPow3(float t, float c) const {
		if (t < c) {
			return 0.0f;
		}
		else {
			return (t - c) * (t - c) * (t - c);
		}
	}

	std::vector<double> Path::GaussianElimination(std::vector<std::vector<double>>& matrix) {
		ReducedEchelonForm(matrix);

		int n = matrix.size();

		std::vector<double> solution(n); // An array to store solution.

		// Start calculating from last equation up to the first.
		for (int i = n - 1; i >= 0; i--) {
			// Start with RHS.
			solution[i] = matrix[i][n];

			// Initialize j to i+1 since matrix is upper triangular.
			for (int j = i + 1; j < n; j++) {
				// Subtract all the lhs values except the coefficient of the variable whose value is being calculated.
				solution[i] -= matrix[i][j] * solution[j];
			}

			// Divide the RHS by the coefficient of the unknown being calculated.
			solution[i] = solution[i] / matrix[i][i];
		}

		return solution;
	}

	void Path::ReducedEchelonForm(std::vector<std::vector<double>>& matrix) {
		int n = matrix.size();

		for (int k = 0; k < n; ++k) {
			// Initialize maximum value and index for pivot.
			int index = k;
			double value = matrix[index][k];

			// Find greater pivot, if any.
			for (int i = k + 1; i < n; ++i) {
				if (glm::abs(matrix[i][k]) > value) {
					value = matrix[i][k];
					index = i;
				}
			}

			// Swap the greatest value row with current row.
			if (index != k) {
				SwapRow(matrix, k, index);
			}

			for (int i = k + 1; i < n; ++i) {
				// Factor f to set current row kth element to 0, and subsequently remaining kth column to 0.
				double f = matrix[i][k] / matrix[k][k];

				// Subtract fth multiple of corresponding kth row element.
				for (int j = k + 1; j <= n; ++j) {
					matrix[i][j] -= matrix[k][j] * f;
				}

				// Filling lower triangular matrix with zeros.
				matrix[i][k] = 0;
			}
		}
	}

	void Path::SwapRow(std::vector<std::vector<double>>& matrix, int i, int j) {
		for (int k = 0; k <= matrix.size(); k++) {
			double temp = matrix[i][k];
			matrix[i][k] = matrix[j][k];
			matrix[j][k] = temp;
		}
	}

	bool Path::IsValid() const {
		return !curveApproximation_.empty();
	}

	void Path::ComputeArcLengthTable() {
		std::size_t numCurvePoints = curveApproximation_.size();
		std::size_t numControlPoints = controlPoints_.size();

		if (numControlPoints == 1) {
			return;
		}

		for (int i = 0; i < numCurvePoints; ++i) {
			float t = ((float)i / (float)(numCurvePoints - 1)) * (float)(numControlPoints - 1);

			if (i == 0) {
				arcLengthTable_.emplace_back(0.0f, 0.0f);
			}
			else {
				glm::vec2 from = curveApproximation_[i - 1];
				glm::vec2 to = curveApproximation_[i];

				// Pairing: (value of t at this point of the curve, arc length)
				arcLengthTable_.emplace_back(t, arcLengthTable_[i - 1].y + glm::length(from - to));
			}
		}

		// Normalize.
		float arcLength = arcLengthTable_.back().y;
		for (int i = 0; i < numCurvePoints; ++i) {
			arcLengthTable_[i].x /= (float)(numControlPoints - 1);
			arcLengthTable_[i].y /= arcLength;
		}
	}

	float Path::GetArcLength(float u) const {
		if (IsValid()) {
			// Difference of t between any two points.
			float du = 1.0f / static_cast<float>(curveLOD_ - 1);

			// Get bounds within the curve points for given value of t
			int index = glm::max(static_cast<int>(std::ceil(u / du)) - 1, 0);
			float ui = arcLengthTable_[index].x;

			float si = arcLengthTable_[index].y;
			float sii = arcLengthTable_[index + 1].y;

			// Compute actual position of point in arc length by interpolating between bounds of t.
			float k = (u - ui) / du;
			return si + k * (sii - si);
		}
		else {
			return 0.0f;
		}
	}

	float Path::GetInterpolationParameter(float s) const {
		if (IsValid()) {
			// Difference of t between any two points.
			float du = 1.0f / static_cast<float>(curveLOD_ - 1);

			// Monotonically increasing entries allows for binary search.
			int start = 0;
			int end = curveLOD_ - 1;

			while (start <= end) {
				int middle = (start + end) / 2;
				if (arcLengthTable_[middle].y <= s) {
					start = middle + 1;
				}
				else {
					end = middle - 1;
				}
			}

			float ui = arcLengthTable_[end].x; // Interpolation parameter.
			float si = arcLengthTable_[end].y; // Arc length.

			float uii = arcLengthTable_[start].x; // Interpolation parameter.
			float sii = arcLengthTable_[start].y; // Arc length.

			// Solve for parameter value using linear interpolation.
			// (u - ui) / (uii - ui) = (s - si) / (sii - si)
			float k = du / (sii - si);
			return ui + k * (s - si);
		}
		else {
			return 0.0f;
		}

	}

	void Path::Clear() {
		controlPoints_.clear();
		Reset();
	}

	void Path::SetControlPoints(const std::vector<glm::dvec2>& controlPoints) {
		controlPoints_ = controlPoints;
		isDirty_ = true;
	}

	void Path::Reset() {
		polynomial_.clear();
		curveApproximation_.clear();
		arcLengthTable_.clear();
		isDirty_ = true;
	}

}

