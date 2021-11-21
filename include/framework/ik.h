
#ifndef SANDBOX_IK_H
#define SANDBOX_IK_H

#include <sandbox_pch.h>
#include <framework/vqs.h>
#include <framework/skeleton.h>

namespace Sandbox {

    class IKSolver {
        public:
            IKSolver();
            ~IKSolver();

            void SolveChain(const std::vector<VQS *> &pose, const glm::vec3 &goalPosition) const;

        private:
            float weight_;
            float error_;

            unsigned maxIterations_;
    };

}


#endif //SANDBOX_IK_H
