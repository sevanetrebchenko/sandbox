
#include <framework/ik.h>

namespace Sandbox {

    IKSolver::IKSolver() : skeleton_(nullptr),
                           weight_(1.0f),
                           error_(0.05f),
                           maxIterations_(20) {
    }

    IKSolver::~IKSolver() = default;

    void IKSolver::SolveChain(const std::vector<std::pair<const Bone &, VQS *>> &pose, std::vector<VQS> transforms, const glm::vec3 &goalPosition) const {
        if (pose.empty() || !skeleton_) {
            return;
        }

        glm::vec3 endEffectorPosition = pose[0].second->GetTranslation();

        // Get the final (!) position of the end effector, given supplied goal position.
        glm::vec3 targetPosition = glm::mix(endEffectorPosition, goalPosition, weight_);

        float squareDistance = 0.0f;
        int iterationCount = 0;

        std::vector<VQS> finalTransformations;

        do {
            // CCD algorithm.
            // First bone is connected to the end effector.
            for (int i = 1; i < pose.size() - 1; ++i) {
                const glm::vec3 &currentJointPosition = pose[i].second->GetTranslation();

                glm::vec3 toEffector = endEffectorPosition - currentJointPosition;
                glm::vec3 toGoal = goalPosition - currentJointPosition;

                // Get angle between two vectors.
                float radians = glm::acos(glm::dot(glm::normalize(toEffector), glm::normalize(toGoal)));

                // Axis is created by crossing the direction to the current joint with the direction to the goal.
                Quaternion rotation(glm::cross(toEffector, toGoal), radians);

                // Hierarchically apply the rotation to the chain.
                pose[i].second->SetOrientation(rotation * pose[i].second->GetOrientation());
            }

//            squareDistance = glm::length2(transforms[pose[0].second->GetTranslation() - targetPosition);
            ++iterationCount;

        } while (squareDistance > error_ && iterationCount <= maxIterations_);
    }

    void IKSolver::SetSkeleton(Skeleton *skeleton) {
        skeleton_ = skeleton;
    }

}