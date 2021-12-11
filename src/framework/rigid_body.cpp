
#include <framework/rigid_body.h>
#include <glm/gtc/random.hpp>

namespace Sandbox {

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

    Spring::Spring(RigidBody *first, RigidBody *second) : start_(first),
                                                          end_(second),
                                                          springCoefficient_(20.f)
                                                          {
        assert(start_ && end_);
        restDistance_ = glm::distance(start_->GetPosition(), end_->GetPosition());
    }

    void Spring::Constrain() const {
        assert(start_ && end_);

        // Get updated distance to constrain by.
        // Positive force if greater than rest distance.
        float d = glm::length(start_->GetPosition() - end_->GetPosition()) - restDistance_;

        float damper = 0.7f;

        // F = -k * d.
        float magnitude = -springCoefficient_ * d;
        {
            glm::vec3 direction = glm::normalize(end_->GetPosition() - start_->GetPosition());
            end_->AddForce(direction * magnitude - damper * end_->GetLinearVelocity());
        }

        {
            glm::vec3 direction = glm::normalize(start_->GetPosition() - end_->GetPosition());
            start_->AddForce(direction * magnitude - damper * start_->GetLinearVelocity());
        }
    }

    RigidBody::RigidBody(const glm::vec3 &position, const glm::mat3 &rotation) : mass(1.0f),
                                                                                 inverseMass(1.0f / mass),
                                                                                 inverseInertiaTensorModel(1.0f),
                                                                                 isFixed_(false),
                                                                                 state_() {
        SetPosition(position);
        SetOrientation(rotation);
    }

    void RigidBody::Update(float dt) {
        if (IsFixed()) {
            return;
        }

        // Integration 1: Force into velocity.
        // Linear.
        state_.linearMomentum += state_.forceAccumulator * dt;
        state_.linearVelocity = state_.linearMomentum * inverseMass;

        // Angular.
        state_.angularMomentum += state_.torqueAccumulator * dt;
        state_.angularVelocity = state_.inverseInertiaTensorWorld * state_.angularMomentum;

        // Integration 2: Velocity into position.

        // Euler.
        // state_.position += state_.linearVelocity * dt;

        // RK2.
		glm::vec3 position1 = state_.position;
		glm::vec3 linearVelocity1 = state_.linearVelocity;
		glm::vec3 angularVelocity1 = state_.angularVelocity;

		// F = ma, therefore a = F / m
		glm::vec3 linearAcceleration1 = state_.forceAccumulator * inverseMass;
		glm::vec3 linearVelocity2 = linearVelocity1 + linearAcceleration1 * dt;

		glm::vec3 angularAcceleration1 = state_.inverseInertiaTensorWorld * state_.torqueAccumulator;
		glm::vec3 angularVelocity2 = angularVelocity1 + angularAcceleration1 * dt;

		state_.position += (linearVelocity1 + linearVelocity2) / 2.0f * dt;

		// Update angular rotation, rotation gets calculated below.
		state_.angularVelocity = (angularVelocity1 + angularVelocity2) / 2.0f * dt;

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

        state_.inverseInertiaTensorWorld = state_.rotation * inverseInertiaTensorModel * glm::transpose(state_.rotation);
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
//        state_.angularMomentum = state_.inverseInertiaTensorWorld * state_.angularVelocity;
    }

    void RigidBody::AddAngularVelocity(const glm::vec3 &angularVelocity) {
        state_.angularVelocity += angularVelocity;
//        state_.angularMomentum = state_.inverseInertiaTensorWorld * state_.angularVelocity;
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
        state_.torqueAccumulator = glm::cross(relativePosition, force); // Force at a point translates into rotation (torque).
    }

    void RigidBody::AddForce(const glm::vec3 &force) {
        state_.forceAccumulator += force; // Force at the COM translates into translation.
    }

    void RigidBody::AddForceAt(const glm::vec3 &force, const glm::vec3 &offset) {
        glm::vec3 relativePosition = offset - state_.position;
        state_.torqueAccumulator += glm::cross(relativePosition, force); // Force at a point translates into rotation (torque).
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
        return inverseMass;
    }

    const glm::mat3 &RigidBody::GetInverseInertiaTensorModel() const {
        return inverseInertiaTensorModel;
    }

    const glm::mat3 &RigidBody::GetInverseInertiaTensorWorld() const {
        return state_.inverseInertiaTensorWorld;
    }

    bool RigidBody::IsFixed() const {
        return isFixed_;
    }

    void RigidBody::SetFixed(bool isFixed) {
        isFixed_ = isFixed;
    }

	RigidBodyCollection::RigidBodyCollection() : useGravity_(false) {
    }

    RigidBodyCollection::~RigidBodyCollection() = default;

    void RigidBodyCollection::Update(float dt) {
        // Apply gravity.
        for (RigidBody& rb : structure_) {
        	if (useGravity_) {
				rb.AddForce(glm::vec3(0.0f, -10.0f, 0.0f));
        	}
            rb.Update(dt);
        }

        for (Spring &spring : connections_) {
            spring.Constrain();
        }
    }

    void RigidBodyCollection::Render() {
    	ddVec3 orange = {1.0f, 0.5f, 0.0f};
    	ddVec3 green = {0.0f, 1.0f, 0.0f};
    	ddVec3 white = {1.0f, 1.0f, 1.0f};

        // Draw masses.
        for (int x = 0; x < dimensions_.x; ++x) {
            for (int y = 0; y < dimensions_.y; ++y) {
                for (int z = 0; z < dimensions_.z; ++z) {
                	RigidBody& rb = structure_[Index(x, y, z)];
                    glm::vec3 position = rb.GetPosition();

                    if (rb.IsFixed()) {
                    	// Draw pivots in green.
                    	dd::point(static_cast<float *>(&position.x), green, 30.0f, 0.0f, false);
                    }
                    else {
                    	// Draw other mass points in orange.
                    	dd::point(static_cast<float *>(&position.x), orange, 30.0f, 0.0f, false);
                    }

                }
            }
        }

        // Draw connections.
        for (Spring &spring : connections_) {
            glm::vec3 start = spring.start_->GetPosition();
            glm::vec3 end = spring.end_->GetPosition();

            dd::line(static_cast<float *>(&start.x), static_cast<float *>(&end.x), white, 0.0f, false);
        }
    }

    void RigidBodyCollection::Preallocate(const glm::vec3 &scale) {
        assert(scale.x > 0 && scale.y > 0 && scale.z > 0);
        scale_ = scale;
        dimensions_ = scale + glm::vec3(1.0f);

        int totalNumPoints = dimensions_[0] * dimensions_[1] * dimensions_[2];
        structure_.resize(totalNumPoints);

        // Generate points.
        GeneratePointStructure();

        // Generate connections.
        GenerateConnectingSprings();
    }

    const RigidBody &RigidBodyCollection::GetRB(int x, int y, int z) const {
        int index = Index(x, y, z);
        assert(index >= 0 && index < structure_.size()); // Validate.
        return structure_[Index(x, y, z)];
    }

    int RigidBodyCollection::Index(int x, int y, int z) const {
        return (z * dimensions_.z * dimensions_.y) + (y * dimensions_.x) + x;
    }

    void RigidBodyCollection::GeneratePointStructure() {
        // Compute local space.
        for (int x = 0; x < dimensions_.x; ++x) {
            for (int y = 0; y < dimensions_.y; ++y) {
                for (int z = 0; z < dimensions_.z; ++z) {
                    structure_[Index(x, y, z)].SetPosition(glm::vec3(x, y, z) - scale_ / 2.0f);
                }
            }
        }

        // Fix points.
        structure_[0].SetFixed(true);
        structure_[structure_.size() - 1].SetFixed(true);
    }

    void RigidBodyCollection::GenerateConnectingSprings() {
        for (int x = 0; x < scale_.x; ++x) {
            for (int y = 0; y < scale_.y; ++y) {
                for (int z = 0; z < scale_.z; ++z) {

                    int index0 = Index(x, y, z);
                    int index1 = Index(x + 1, y, z);
                    int index2 = Index(x, y + 1, z);
                    int index3 = Index(x + 1, y + 1, z);
                    int index4 = Index(x, y, z + 1);
                    int index5 = Index(x + 1, y, z + 1);
                    int index6 = Index(x, y + 1, z + 1);
                    int index7 = Index(x + 1, y + 1, z + 1);

                    // Generate cube corner connectors.
                    connections_.emplace_back(&structure_[index0], &structure_[index1]);
                    connections_.emplace_back(&structure_[index0], &structure_[index2]);
                    connections_.emplace_back(&structure_[index2], &structure_[index3]);
                    connections_.emplace_back(&structure_[index1], &structure_[index3]);

                    connections_.emplace_back(&structure_[index0], &structure_[index4]);
                    connections_.emplace_back(&structure_[index1], &structure_[index5]);
                    connections_.emplace_back(&structure_[index2], &structure_[index6]);
                    connections_.emplace_back(&structure_[index3], &structure_[index7]);

                    connections_.emplace_back(&structure_[index4], &structure_[index5]);
                    connections_.emplace_back(&structure_[index4], &structure_[index6]);
                    connections_.emplace_back(&structure_[index6], &structure_[index7]);
                    connections_.emplace_back(&structure_[index5], &structure_[index7]);

                    // Generate cube face diagonal connectors.
                    connections_.emplace_back(&structure_[index0], &structure_[index5]);
                    connections_.emplace_back(&structure_[index1], &structure_[index4]);

                    connections_.emplace_back(&structure_[index1], &structure_[index7]);
                    connections_.emplace_back(&structure_[index3], &structure_[index5]);

                    connections_.emplace_back(&structure_[index0], &structure_[index6]);
                    connections_.emplace_back(&structure_[index2], &structure_[index4]);

                    connections_.emplace_back(&structure_[index0], &structure_[index3]);
                    connections_.emplace_back(&structure_[index1], &structure_[index2]);

                    connections_.emplace_back(&structure_[index4], &structure_[index7]);
                    connections_.emplace_back(&structure_[index5], &structure_[index6]);

                    connections_.emplace_back(&structure_[index3], &structure_[index6]);
                    connections_.emplace_back(&structure_[index2], &structure_[index7]);

                    // Generate cube internal diagonal connectors.
                    connections_.emplace_back(&structure_[index0], &structure_[index7]);
                    connections_.emplace_back(&structure_[index4], &structure_[index3]);
                    connections_.emplace_back(&structure_[index1], &structure_[index6]);
                    connections_.emplace_back(&structure_[index2], &structure_[index5]);

                }
            }
        }
    }

	void RigidBodyCollection::OnImGui() {
    	if (ImGui::Begin("Rigid Body Collection")) {
    		ImGui::PushStyleColor(ImGuiCol_Text, 0xff999999);
				ImGui::Text("Number of Mass Points: %zu", structure_.size());
				ImGui::Text("Number of Springs: %zu", connections_.size());
    		ImGui::PopStyleColor();

    		if (ImGui::Button("Apply Random Force")) {
    			// Pick random.
    			int index = glm::linearRand(0, (int)structure_.size() - 1);
    			structure_[index].AddForce(glm::vec3(glm::linearRand(0.5f, 1.0f) * 250.0f));
    		}

    		ImGui::Text("Apply Gravity?");
    		ImGui::Checkbox("##gravity", &useGravity_);

    		// Anchor positions.
    		ImGui::Text("Anchor 1 Position: ");
    		glm::vec3 anchor1Position = structure_[0].GetPosition();
    		if (ImGui::DragFloat3("##anchor1", &anchor1Position.x, 0.05, -10.0f, 10.0f)) {
    			structure_[0].SetPosition(anchor1Position);
    		}

    		ImGui::Text("Anchor 2 Position: ");
    		glm::vec3 anchor2Position = structure_[structure_.size() - 1].GetPosition();
    		if (ImGui::DragFloat3("##anchor2", &anchor2Position.x, 0.05, -10.0f, 10.0f)) {
    			structure_[structure_.size() - 1].SetPosition(anchor2Position);
    		}

    		ImGui::End();
    	}
	}
}
