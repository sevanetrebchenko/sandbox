
#include <framework/rigid_body.h>

namespace Sandbox {

    void PrintVec3(std::string preface, const glm::vec3& vector) {
        std::cout << preface << ": (" << vector.x << ", " << vector.y << ", " << vector.z << ")" << std::endl;
    }

    State::State() : position(0.0f),
                     rotation(1.0f),
                     linearVelocity(0.0f),
                     angularVelocity(0.0f),
                     forceAccumulator(0.0f),
                     torqueAccumulator(0.0f),
                     linearMomentum(0.0f),
                     angularMomentum(0.0f),
                     inverseInertiaTensorWorld(1.0f) {

    }

    Shape::Shape() : mass(8.0f),
                     inverseMass(1.0f / 8.0f), // Assume mass of 1.0 per vertex.
                     inverseInertiaTensorModel(1.0f) {
        // Assume cube of length 1 unit per side.
        float x = 1.0f;
        float y = 1.0f;
        float z = 1.0f;

        // Construct the inertia tensor (in object space).
        inverseInertiaTensorModel[0][0] = mass * (y * y) + (z * z);
        inverseInertiaTensorModel[1][1] = mass * (x * x) + (z * z);
        inverseInertiaTensorModel[2][2] = mass * (x * x) + (y * y);
    }

    RigidBody::RigidBody(const glm::vec3 &position, const glm::mat3 &rotation) : state_(),
                                                                                 shape_(),
                                                                                 isFixed_(false) {
        SetPosition(position);
        SetOrientation(rotation);
    }

    void RigidBody::Update(float dt) {
        if (IsFixed()) {
            return;
        }

        PrintVec3("force accumulator", state_.forceAccumulator);
        PrintVec3("torque accumulator", state_.torqueAccumulator);

        // Integration 1: Force into velocity.
        // Linear.
        state_.linearMomentum += state_.forceAccumulator * dt;
        state_.linearVelocity += state_.linearMomentum * shape_.inverseMass;

        PrintVec3("linear momentum", state_.linearMomentum);
        PrintVec3("linear velocity", state_.linearVelocity);

        // Angular.
        state_.angularMomentum += state_.torqueAccumulator * dt;
        state_.angularVelocity = state_.inverseInertiaTensorWorld * state_.angularMomentum;

        PrintVec3("angular momentum", state_.angularMomentum);
        PrintVec3("angular velocity", state_.angularVelocity);

        // Integration 2: Velocity into position.
        // Update position.
        state_.position += dt * state_.linearVelocity;

        PrintVec3("position", state_.position);

        // Update rotation.
        glm::mat3 rotation;

        // Construct rotation matrix from angular velocity using the tilde operator.
        rotation[0][0] = 0.0f;
        rotation[0][1] = state_.angularVelocity.z;
        rotation[0][2] = -state_.angularVelocity.y;

        rotation[1][0] = -state_.angularVelocity.z;
        rotation[1][1] = 0.0f;
        rotation[1][2] = state_.angularVelocity.x;

        rotation[2][0] = state_.angularVelocity.y;
        rotation[2][1] = -state_.angularVelocity.x;
        rotation[2][2] = 0.0f;

        state_.rotation += rotation * state_.rotation * dt;

        // Orthonormalize orientation matrix.
        glm::vec3 x = glm::normalize(state_.rotation[0]);
        glm::vec3 y = state_.rotation[1];
        glm::vec3 z = glm::normalize(glm::cross(x, y));
        y = glm::normalize(glm::cross(z, x));
        state_.rotation[0] = x;
        state_.rotation[1] = y;
        state_.rotation[2] = z;

        // Processing setup for next frame.
        state_.forceAccumulator = glm::vec3(0.0f);
        state_.torqueAccumulator = glm::vec3(0.0f);

        std::cout << "finished frame" << std::endl;
        std::cout << std::endl;
    }

    const glm::vec3 &RigidBody::GetPosition() const {
        return state_.position;
    }

    void RigidBody::SetPosition(const glm::vec3 &position) {
        state_.position = position;
    }

    void RigidBody::AddPosition(const glm::vec3 &position) {
        state_.position += position;
    }

    const glm::mat3 &RigidBody::GetOrientation() const {
        return state_.rotation;
    }

    void RigidBody::SetOrientation(const glm::mat3 &orientation) {
        state_.rotation = orientation;
    }

    const glm::vec3 &RigidBody::GetLinearVelocity() const {
        return state_.linearVelocity;
    }

    void RigidBody::SetLinearVelocity(const glm::vec3 &linearVelocity) {
        state_.linearVelocity = linearVelocity;
    }

    void RigidBody::AddLinearVelocity(const glm::vec3 &linearVelocity) {
        state_.linearVelocity += linearVelocity;
    }

    const glm::vec3 &RigidBody::GetAngularVelocity() const {
        return state_.angularVelocity;
    }

    void RigidBody::SetAngularVelocity(const glm::vec3 &angularVelocity) {
        state_.angularVelocity = angularVelocity;
        state_.angularMomentum = state_.inverseInertiaTensorWorld * state_.angularVelocity;
    }

    void RigidBody::AddAngularVelocity(const glm::vec3 &angularVelocity) {
        state_.angularVelocity += angularVelocity;
        state_.angularMomentum = state_.inverseInertiaTensorWorld * state_.angularVelocity;
    }

    const glm::vec3 &RigidBody::GetLinearMomentum() const {
        return state_.linearMomentum;
    }

    void RigidBody::SetLinearMomentum(const glm::vec3 &linearMomentum) {
        state_.linearMomentum = linearMomentum;
    }

    void RigidBody::AddLinearMomentum(const glm::vec3 &linearMomentum) {
        state_.linearMomentum += linearMomentum;
    }

    const glm::vec3 &RigidBody::GetAngularMomentum() const {
        return state_.angularMomentum;
    }

    void RigidBody::SetAngularMomentum(const glm::vec3 &angularMomentum) {
        state_.angularMomentum = angularMomentum;
    }

    void RigidBody::AddAngularMomentum(const glm::vec3 &angularMomentum) {
        state_.angularMomentum += angularMomentum;
    }

    const glm::vec3 &RigidBody::GetTotalForce() const {
        return state_.forceAccumulator;
    }

    void RigidBody::SetForce(const glm::vec3 &force) {
        state_.forceAccumulator = force;
    }

    void RigidBody::SetForceAt(const glm::vec3 &force, const glm::vec3 &offset) {
        glm::vec3 relativePosition = offset - state_.position;
        state_.torqueAccumulator = glm::cross(relativePosition,
                                              force); // Force at a point translates into rotation (torque).
    }

    void RigidBody::AddForce(const glm::vec3 &force) {
        state_.forceAccumulator += force; // Force at the COM translates into translation.
    }

    void RigidBody::AddForceAt(const glm::vec3 &force, const glm::vec3 &offset) {
        glm::vec3 relativePosition = offset - state_.position;
        state_.torqueAccumulator += glm::cross(relativePosition,
                                               force); // Force at a point translates into rotation (torque).
    }

    const glm::vec3 &RigidBody::GetTotalTorque() const {
        return state_.torqueAccumulator;
    }

    void RigidBody::SetTorque(const glm::vec3 &torque) {
        state_.torqueAccumulator = torque;
    }

    void RigidBody::AddTorque(const glm::vec3 &torque) {
        state_.torqueAccumulator += torque;
    }

    float RigidBody::GetInverseMass() const {
        return shape_.inverseMass;
    }

    const glm::mat3 &RigidBody::GetInverseInertiaTensorModel() const {
        return shape_.inverseInertiaTensorModel;
    }

    const glm::mat3 &RigidBody::GetInverseInertiaTensorWorld() const {
        return state_.inverseInertiaTensorWorld;
    }

    bool RigidBody::IsFixed() const {
        return isFixed_;
    }

    void RigidBody::SetFixed(bool fixed) {
        isFixed_ = fixed;
    }

}
