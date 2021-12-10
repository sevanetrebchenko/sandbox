
#ifndef SANDBOX_RIGID_BODY_H
#define SANDBOX_RIGID_BODY_H

#include <sandbox_pch.h>

namespace Sandbox {

    struct State {
        State();
        ~State() = default;

        glm::vec3 position; // Position of the center of mass.
        glm::mat3 rotation;

        glm::vec3 linearVelocity;
        glm::vec3 angularVelocity;

        glm::vec3 forceAccumulator;
        glm::vec3 torqueAccumulator;

        glm::vec3 linearMomentum;
        glm::vec3 angularMomentum;

        glm::mat3 inverseInertiaTensorWorld;
    };

    struct Shape {
        Shape();
        ~Shape() = default;

        float mass;
        float inverseMass;
        glm::mat3 inverseInertiaTensorModel;
    };


    class RigidBody {
        public:
            RigidBody(const glm::vec3& position = glm::vec3(0.0f), const glm::mat3& rotation = glm::mat3(1.0f));
            ~RigidBody() = default;

            void Update(float dt);

            [[nodiscard]] const glm::vec3& GetPosition() const;
            void SetPosition(const glm::vec3& position);
            void AddPosition(const glm::vec3& position);

            [[nodiscard]] const glm::mat3& GetOrientation() const;
            void SetOrientation(const glm::mat3& orientation);

            [[nodiscard]] const glm::vec3& GetLinearVelocity() const;
            void SetLinearVelocity(const glm::vec3& linearVelocity);
            void AddLinearVelocity(const glm::vec3& linearVelocity);

            [[nodiscard]] const glm::vec3& GetAngularVelocity() const;
            void SetAngularVelocity(const glm::vec3& angularVelocity);
            void AddAngularVelocity(const glm::vec3& angularVelocity);

            [[nodiscard]] const glm::vec3& GetLinearMomentum() const;
            void SetLinearMomentum(const glm::vec3& linearMomentum);
            void AddLinearMomentum(const glm::vec3& linearMomentum);

            [[nodiscard]] const glm::vec3& GetAngularMomentum() const;
            void SetAngularMomentum(const glm::vec3& angularMomentum);
            void AddAngularMomentum(const glm::vec3& angularMomentum);

            [[nodiscard]] const glm::vec3& GetTotalForce() const;
            void SetForce(const glm::vec3& force);
            void SetForceAt(const glm::vec3& force, const glm::vec3& offset);
            void AddForce(const glm::vec3& force);
            void AddForceAt(const glm::vec3& force, const glm::vec3& offset);

            [[nodiscard]] const glm::vec3& GetTotalTorque() const;
            void SetTorque(const glm::vec3& torque);
            void AddTorque(const glm::vec3& torque);

            [[nodiscard]] float GetInverseMass() const;
            [[nodiscard]] const glm::mat3& GetInverseInertiaTensorModel() const;
            [[nodiscard]] const glm::mat3& GetInverseInertiaTensorWorld() const;

            [[nodiscard]] bool IsFixed() const;
            void SetFixed(bool fixed);

        private:
            State state_;
            Shape shape_;

            bool isFixed_;
    };

}

#endif //SANDBOX_RIGID_BODY_H
