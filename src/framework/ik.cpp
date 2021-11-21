
#include <framework/ik.h>

namespace Sandbox {

    IKSolver::IKSolver() : weight_(1.0f),
                           error_(0.05f),
                           maxIterations_(15) {
    }

    IKSolver::~IKSolver() = default;

    void IKSolver::SolveChain(const std::vector<VQS *> &pose, const glm::vec3 &goalPosition) const {
        if (pose.empty()) {
            return;
        }

        glm::vec3 endEffectorPosition = pose[0]->GetTranslation();

        // Get the final (!) position of the end effector, given supplied goal position.
        glm::vec3 targetPosition = glm::mix(endEffectorPosition, goalPosition, weight_);

        float squareDistance = 0.0f;
        int iterationCount = 0;

        do {
            // CCD algorithm.
            // First bone is connected to the end effector.
            for (int i = 1; i < pose.size(); ++i) {
                const glm::vec3 &currentJointPosition = pose[i]->GetTranslation();

                glm::vec3 toEffector = endEffectorPosition - currentJointPosition;
                glm::vec3 toGoal = goalPosition - currentJointPosition;

                // Get angle between two vectors.
                float radians = glm::acos(glm::dot(glm::normalize(toEffector), glm::normalize(toGoal)));

                // Axis is created by crossing the direction to the current joint with the direction to the goal.
                Quaternion rotation(glm::cross(toEffector, toGoal), radians);

                // Hierarchically apply the rotation to the chain.
                pose[i]->SetOrientation(rotation * pose[i]->GetOrientation());
            }

            squareDistance = glm::length2(pose[0]->GetTranslation() - targetPosition);
            ++iterationCount;
        } while (squareDistance > error_ && iterationCount <= maxIterations_);
    }

}