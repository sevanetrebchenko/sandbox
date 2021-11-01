
// Scenes.
#include <scenes/project1/project1.h>
#include <scenes/project2/project2.h>
#include <scenes/deferred_rendering/deferred_rendering.h>



std::vector<glm::vec2> points{};
std::vector<glm::vec2> curve{};

std::vector<glm::vec2> polynomial{};

// Truncated power function (t - c) ^ 3+
float TruncatedPow3(float t, float c) {
    if (t < c) {
        return 0.0f;
    } else {
        return (t - c) * (t - c) * (t - c);
    }
}

glm::vec2 Evaluate(float t) {
    glm::vec2 value(0.0f);

    // a0 = 1
    value.x += polynomial[0].x;
    value.y += polynomial[0].y;

    // a1 = t
    value.x += polynomial[1].x * t;
    value.y += polynomial[1].y * t;

    //a2 = t ^ 2
    value.x += polynomial[2].x * t * t;
    value.y += polynomial[2].y * t * t;

    //a3 = t ^ 3
    value.x += polynomial[3].x * t * t * t;
    value.y += polynomial[3].y * t * t * t;

    // Truncated power functions.
    for (int c = 1; c <= polynomial.size() - 4; ++c) {
        float multiplier = TruncatedPow3(t, c);
        value.x += polynomial[3 + c].x * multiplier;
        value.y += polynomial[3 + c].y * multiplier;
    }

    return value;
}

void SwapRow(std::vector<std::vector<double>> &matrix, int n, int i, int j) {
    for (int k = 0; k <= n; k++) {
        double temp = matrix[i][k];
        matrix[i][k] = matrix[j][k];
        matrix[j][k] = temp;
    }
}

void ReducedEchelonForm(std::vector<std::vector<double>> &matrix, int n) {
    for (int k = 0; k < n; k++) {
        // Initialize maximum value and index for pivot.
        int i_max = k;
        double v_max = matrix[i_max][k];

        // Find greater amplitude for pivot if any.
        for (int i = k + 1; i < n; i++) {
            if (glm::abs(matrix[i][k]) > v_max) {
                v_max = matrix[i][k];
                i_max = i;
            }
        }

        // Swap the greatest value row with current row
        if (i_max != k) {
            SwapRow(matrix, n, k, i_max);
        }

        for (int i = k + 1; i < n; i++) {
            // Factor f to set current row kth element to 0, and subsequently remaining kth column to 0.
            double f = matrix[i][k] / matrix[k][k];

            // Subtract fth multiple of corresponding kth row element.
            for (int j = k + 1; j <= n; j++) {
                matrix[i][j] -= matrix[k][j] * f;
            }

            // Filling lower triangular matrix with zeros.
            matrix[i][k] = 0;
        }
    }
}

// Gaussian Elimination algorithm repurposed from: https://www.geeksforgeeks.org/gaussian-elimination/
std::vector<double> Solve(const std::vector<std::vector<double>> &matrix, int n) {
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

// Gaussian Elimination algorithm repurposed from: https://www.geeksforgeeks.org/gaussian-elimination/
std::vector<double> GaussianElimination(std::vector<std::vector<double>> &matrix, int n) {
    ReducedEchelonForm(matrix, n);
    return Solve(matrix, n);
}

void InterpolatingSplines() {
    int numPoints = points.size();

    // No curve with 1 point.
    if (numPoints <= 1) {
        return;
    }

    int k = numPoints - 1; // 0, 1, 2, 3, 4, .... k (k will be n - 1)
    int numCoefficients = numPoints + 2; // numPoints + f''(0) and f''(k)

    std::vector<std::vector<glm::vec2>> linearSystem;
    linearSystem.resize(numCoefficients);

    // Preallocate.
    for (std::vector<glm::vec2> &set : linearSystem) {
        set.resize(numCoefficients + 1);
    }

    // Initialize points inside linear system.
    for (int i = 0; i < numCoefficients; ++i) {
        for (int j = 0; j < numCoefficients + 1; ++j) {
            linearSystem[i][j] = glm::vec2(0.0f);
        }
    }

    for (int i = 0; i < numPoints; ++i) {
        // From 0 -> 1 is just a regular cubic function.
        linearSystem[i][0] = glm::vec2(1, 1);                 // a0 -> 1
        linearSystem[i][1] = glm::vec2(i, i);                 // a1 -> t
        linearSystem[i][2] = glm::vec2(i * i, i * i);         // a2 -> t^2
        linearSystem[i][3] = glm::vec2(i * i * i, i * i * i); // a3 -> t^3

        // Shifted power basis from c = 1 -> k - 1
        for (int c = 1; c <= k - 1; ++c) {
            float value = TruncatedPow3(static_cast<float>(i), static_cast<float>(c));
            linearSystem[i][3 + c] = glm::vec2(value, value);
        }

        // Emplace data point.
        linearSystem[i][numCoefficients] = points[i];
    }

    // Emplace f''(0) = 0 and f''(k) = 0 as the last 2 equations.
    // f''(0) = 0
    linearSystem[numPoints][0] = glm::vec2(0, 0); // a0       -> 0
    linearSystem[numPoints][1] = glm::vec2(0, 0); // a1 t     -> 0
    linearSystem[numPoints][2] = glm::vec2(2, 2); // a2 t ^ 2 -> 2

    for (int i = 3; i <= numPoints + 1; ++i) {
        linearSystem[numPoints][i] = glm::vec2(0, 0); // All other variables are 0.
    }

    // f''(k) = 0
    linearSystem[numPoints + 1][0] = glm::vec2(0, 0); // a0        -> 0
    linearSystem[numPoints + 1][1] = glm::vec2(0, 0); // a1 t      -> 0
    linearSystem[numPoints + 1][2] = glm::vec2(2, 2); // a2 t ^ 2  -> 2
    linearSystem[numPoints + 1][3] = glm::vec2(6 * k, 6 * k); // a3 t ^ 3  ->  6t

    for (int i = 1; i <= k - 1; ++i) {
        if (k - i < 0) {
            linearSystem[numPoints + 1][3 + i] = glm::vec2(0, 0);
        } else {
            linearSystem[numPoints + 1][3 + i] = glm::vec2(6 * (k - i), 6 * (k - i));
        }
    }

    // Emplace final 0.
    linearSystem[numPoints + 1][numCoefficients] = glm::vec2(0, 0);


    // Solve system using Gaussian elimination.
    std::vector<std::vector<double>> xCoefficients{};
    xCoefficients.resize(numCoefficients);
    for (std::vector<double> &set : xCoefficients) {
        set.resize(numCoefficients + 1);
    }

    std::vector<std::vector<double>> yCoefficients{};
    yCoefficients.resize(numCoefficients);
    for (std::vector<double> &set : yCoefficients) {
        set.resize(numCoefficients + 1);
    }

    // Emplace initial data.
    for (int i = 0; i < numCoefficients; ++i) {
        for (int j = 0; j < numCoefficients + 1; ++j) {
            xCoefficients[i][j] = linearSystem[i][j].x;
            yCoefficients[i][j] = linearSystem[i][j].y;
        }
    }

    std::vector<double> xSolution = GaussianElimination(xCoefficients, numCoefficients);
    std::vector<double> ySolution = GaussianElimination(yCoefficients, numCoefficients);

    polynomial.resize(numCoefficients);

    // Emplace solution.
    for (int i = 0; i < numCoefficients; ++i) {
        polynomial[i] = glm::vec2(xSolution[i], ySolution[i]);
    }

    // Calculate curve.
    int steps = numPoints * 15;
    float alpha = static_cast<float>(numPoints - 1) / static_cast<float>(steps);

    for (int i = 0; i <= steps; ++i) {
        curve.push_back(Evaluate(alpha * static_cast<float>(i)));
    }
}



int main() {

    points.emplace_back(0.0f, 0.0f);
    points.emplace_back(10.0f, 5.0f);
    points.emplace_back(6.0f, 4.0f);
    points.emplace_back(3.0f, 1.0f);
    points.emplace_back(7.0f, -8.0f);
    points.emplace_back(14.0f, -4.0f);
    points.emplace_back(10.0f, -12.0f);

    InterpolatingSplines();

    Sandbox::Scene *scene = new Sandbox::SceneProject2(1280, 720);
    try {
        scene->Init();
    }
    catch (std::runtime_error &exception) {
        std::cerr << exception.what() << std::endl;
        return 1;
    }

    scene->Run();
    scene->Shutdown();

    return 0;
}

#include <glm/glm.hpp>

