
#include <framework/animator.h>
#include <framework/skinned_mesh.h>
#include <glm/gtx/quaternion.hpp>
#include <framework/interpolation.h>

namespace Sandbox {

    Animator::Animator() : _target(nullptr),
                           _selectedAnimation(nullptr),
                           _keyInterpolationMethod(KeyInterpolationMethod::DEFAULT),
                           _quaternionInterpolationMethod(QuaternionInterpolationMethod::SLERP),
                           endEffectorIndex_(-1),
                           targetPosition_(glm::vec3(0.0f)),
                           _currentTime(0.0f),
                           _playbackSpeed(1.0f),
                           _useBindPose(false),
                           _reset(false)
                           {
    }

    Animator::~Animator() = default;

    void Animator::SetTarget(Skeleton *skeleton) {
        _target = skeleton;

        std::size_t numBones = skeleton->_bones.size();
        boneVQSTransformations_.resize(numBones);
    }

    void Animator::AddAnimation(Animation *animation) {
        _animations.push_back(animation);
        _selectedAnimation = animation;
    }

    void Animator::PlayAnimation(const std::string &animationName) {
        for (Animation* animation : _animations) {
            if (animation->_name == animationName) {
                _selectedAnimation = animation;
                break;
            }
        }
    }

    void Animator::PlayAnimation(unsigned animationIndex) {
        if (!_animations.empty()) {
            // Bounds check.
            if (animationIndex < _animations.size()) {
                _selectedAnimation = _animations[animationIndex];
            }
        }
    }

    void Animator::InterpolateBone(int boneIndex) {
        switch (_keyInterpolationMethod) {
            case KeyInterpolationMethod::DEFAULT:
                DefaultKeyInterpolation(boneIndex);
                break;
            case KeyInterpolationMethod::INCREMENTAL:
                IncrementalKeyInterpolation(boneIndex);
                break;
        }
    }

    const std::vector<VQS> &Animator::GetBoneTransformations() const {
        return boneVQSTransformations_;
    }

    const Animation *Animator::GetCurrentAnimation() const {
        return _selectedAnimation;
    }

    float Animator::GetPlaybackSpeed() const {
        return _playbackSpeed;
    }

    void Animator::SetPlaybackSpeed(float playbackSpeed) {
        _playbackSpeed = playbackSpeed;
    }

    bool Animator::IsBindPoseActive() const {
        return _useBindPose;
    }

    void Animator::SetBindPoseActive(bool active) {
        _useBindPose = active;
    }

    KeyInterpolationMethod Animator::GetKeyInterpolationMethod() const {
        return _keyInterpolationMethod;
    }

    void Animator::SetKeyInterpolationMethod(KeyInterpolationMethod method) {
        _keyInterpolationMethod = method;
    }

    QuaternionInterpolationMethod Animator::GetQuaternionInterpolationMethod() const {
        return _quaternionInterpolationMethod;
    }

    void Animator::SetQuaternionInterpolationMethod(QuaternionInterpolationMethod method) {
        _quaternionInterpolationMethod = method;
    }

    void Animator::DefaultKeyInterpolation(int boneIndex) {
        // Compute interpolated (local) transformations for this bone.
        std::pair<KeyPosition, KeyPosition> positionKeys = _selectedAnimation->GetPositionKeyPair(boneIndex, _currentTime);
        std::pair<KeyRotation, KeyRotation> rotationKeys = _selectedAnimation->GetRotationKeyPair(boneIndex, _currentTime);
        std::pair<KeyScale, KeyScale> scaleKeys = _selectedAnimation->GetScaleKeyPair(boneIndex, _currentTime);

        VQS finalVQS { };

        // Interpolate scale.
        {
            finalVQS.SetScalingFactor(1.0f); // Scaling does not change.
        }

        // Interpolate rotation.
        {
            // Get time interval.
            const KeyRotation& start = rotationKeys.first;
            const KeyRotation& end = rotationKeys.second;

            float startTime = start.dt;
            float endTime = end.dt;

            // Get rotations.
            const Quaternion& first = start.orientation;
            const Quaternion& second = end.orientation;

            if (glm::all(glm::epsilonEqual(first.ToVec4(), second.ToVec4(), std::numeric_limits<float>::epsilon()))) {
                finalVQS.SetOrientation(first);
            }
            else {
                // Normalize the distance between the two keyframes.
                float length = endTime - startTime;
                float current = glm::clamp(_currentTime - startTime, 0.0f, _selectedAnimation->_duration);
                float t = current / length;

                switch (_quaternionInterpolationMethod) {
                    case QuaternionInterpolationMethod::LERP:
                        finalVQS.SetOrientation(Lerp(first, second, t));
                        break;
                        case QuaternionInterpolationMethod::SLERP:
                            finalVQS.SetOrientation(Slerp(first, second, t));
                            break;
                }
            }
        }

        // Interpolate position.
        {
            // Get time interval.
            const KeyPosition& start = positionKeys.first;
            const KeyPosition& end = positionKeys.second;

            if (glm::all(glm::epsilonEqual(start.position, end.position, std::numeric_limits<float>::epsilon()))) {
                finalVQS.SetTranslation(start.position);
            }
            else {
                float startTime = start.dt;
                float endTime = end.dt;

                // Normalize the distance between the two keyframes.
                float length = endTime - startTime;
                float current = glm::clamp(_currentTime - startTime, 0.0f, _selectedAnimation->_duration);
                float t = current / length;

                finalVQS.SetTranslation((1.0f - t) * start.position + t * end.position); // Lerp.
            }
        }

        boneVQSTransformations_[boneIndex] = finalVQS;
    }

    void Animator::IncrementalKeyInterpolation(int boneIndex) {
        // Compute interpolated (local) transformations for this bone.
        std::pair<KeyPosition, KeyPosition> positionKeys = _selectedAnimation->GetPositionKeyPair(boneIndex, _currentTime);
        std::pair<KeyRotation, KeyRotation> rotationKeys = _selectedAnimation->GetRotationKeyPair(boneIndex, _currentTime);
        std::pair<KeyScale, KeyScale> scaleKeys = _selectedAnimation->GetScaleKeyPair(boneIndex, _currentTime);

        VQS finalVQS { };
        unsigned numSteps = 20;

        // Interpolate scale.
        {
            finalVQS.SetScalingFactor(1.0f); // Scaling does not change.
        }

        // Interpolate rotation.
        {
            // Get time interval.
            const KeyRotation& start = rotationKeys.first;
            const KeyRotation& end = rotationKeys.second;

            float startTime = start.dt;
            float endTime = end.dt;

            // Get rotations.
            const Quaternion& first = start.orientation;
            const Quaternion& second = end.orientation;

            // Normalize the distance between the two keyframes.
            float length = endTime - startTime;
            float current = glm::clamp(_currentTime - startTime, 0.0f, _selectedAnimation->_duration);
            float t = current / length;

            switch (_quaternionInterpolationMethod) {
                case QuaternionInterpolationMethod::LERP:
                    finalVQS.SetOrientation(iLerp(first, second, t, numSteps));
                    break;
                case QuaternionInterpolationMethod::SLERP:
                    finalVQS.SetOrientation(iSlerp(first, second, t, numSteps));
                    break;
            }
        }

        // Interpolate position.
        {
            // Get time interval.
            const KeyPosition& start = positionKeys.first;
            const KeyPosition& end = positionKeys.second;

            float startTime = start.dt;
            float endTime = end.dt;

            // Normalize the distance between the two keyframes.
            float length = endTime - startTime;
            float current = glm::clamp(_currentTime - startTime, 0.0f, _selectedAnimation->_duration);
            float t = current / length;

            // Interpolate incrementally.
            glm::vec3 step = (end.position - start.position) * (1.0f / static_cast<float>(numSteps));
            float to = t * static_cast<float>(numSteps);

            finalVQS.SetTranslation(start.position + step * to); // iLerp.
        }

        boneVQSTransformations_[boneIndex] = finalVQS;
    }

    void Animator::ComputeLocalBoneTransformations(float dt) {
        // Loop animation forever.
        _currentTime += dt * _selectedAnimation->_speed * _playbackSpeed;
        _currentTime = std::fmod(_currentTime, _selectedAnimation->_duration);

        // Computes local transformations.
        for (Bone& bone : _target->_bones) {
            InterpolateBone(bone._index);
        }
    }

    void Animator::ComputeFinalBoneTransformations() {
        for (Bone& bone : _target->_bones) {
            int boneIndex = bone._index;

            // Compute final transformation for this bone.
            VQS parentTransform { };
            if (bone._parentIndex != -1) {
                parentTransform = boneVQSTransformations_[bone._parentIndex];
            }

            // Apply parent transformation on top of bone local transformation.
            boneVQSTransformations_[boneIndex] = parentTransform * boneVQSTransformations_[boneIndex];
        }
    }

    bool Animator::ProcessBindPose() {
        if (!_target || !_selectedAnimation) {
            return false;
        }

        if (_useBindPose) {
            // This needs to be done only once.
            if (!_reset) {
                // Need to reset bone positions to bind position.
                for (std::size_t i = 0; i < _target->_bones.size(); ++i) {
                    // Use identity VQS.
                    boneVQSTransformations_[i] = VQS();
                }
            }

            return false;
        }

        _reset = _useBindPose;
        return true;
    }

    int Animator::GetEndEffectorBoneIndex() const {
        return endEffectorIndex_;
    }

    void Animator::SetEndEffectorBoneIndex(int index) {
        if (endEffectorIndex_ != index) {
            endEffectorIndex_ = index;
            ComputeIKChain();
        }
    }

    void Animator::ComputeIKChain() {
        // Find desired end-effector bone index.
        chain_.clear();

        // Emplace chain starting at end effector and ending at the root.
        int index = endEffectorIndex_;
        while (index != 0) {
            Bone& bone = _target->_bones[index];
            chain_.emplace_back(index);
            index = bone._parentIndex;
        }
    }

    const glm::vec3 &Animator::GetIKTargetPosition() const {
        return targetPosition_;
    }

    void Animator::SetIKTargetPosition(const glm::vec3& targetPosition) {
        targetPosition_ = targetPosition;
    }

    void Animator::SolveIKChainCCD(const glm::mat4& modelMatrix) {
        glm::mat4 modelInverse = glm::inverse(modelMatrix);
        const std::size_t numChainElements = chain_.size();

        glm::vec3 chainOrigin = GetGlobalTransformation(chain_[numChainElements - 1]).GetTranslation(); // Model space.
        glm::vec3 endEffectorPosition = modelInverse * glm::vec4(GetGlobalTransformation(chain_[0]).GetTranslation(), 1.0f); // Model space.

        const float bias = 0.75f; // In case bones cannot fully extend.
        float chainLength = GetIKChainLength() * bias;

        glm::vec3 goalPosition = modelInverse * glm::vec4(targetPosition_, 1.0f);

//        // Place goal position within reach of the IK chain.
//        if (glm::distance(chainOrigin, targetPosition_) > chainLength) {
//            glm::vec3 direction = glm::normalize(targetPosition_ - chainOrigin);
//            goalPosition = chainOrigin + direction * chainLength;
//        }

        dd::sphere(static_cast<const float*>(&goalPosition[0]), dd::colors::Magenta, 0.1f, 0, false);

        const float epsilon = 0.2f;
        const int numIterations = 10;

        float distance = glm::length(endEffectorPosition - goalPosition);

        // End effector is already at the goal position.
        if (distance < epsilon) {
            return;
        }

        int iterationCount = 0;
        do {
            // CCD algorithm.
            // First bone is connected to the end effector.
            // Last bone is the root.
            int limit = 2;

            while (limit != chain_.size()) {
                for (int i = 1; i < limit; ++i) {
                    int boneIndex = chain_[i];
                    VQS currentJointTransformation = GetGlobalTransformation(boneIndex);
                    glm::vec3 currentJointPosition = currentJointTransformation.GetTranslation(); // Model space.

                    glm::vec3 toEffector = glm::normalize(endEffectorPosition - currentJointPosition);
                    glm::vec3 toGoal = glm::normalize(goalPosition - currentJointPosition);

                    float angle = glm::degrees(glm::acos(glm::dot(toEffector, toGoal)));
                    glm::vec3 axis = glm::cross(toEffector, toGoal);

                    Quaternion rotation { axis, angle }; // Rotation to get from current to desired orientation.

                    const Quaternion& current = boneVQSTransformations_[boneIndex].GetOrientation();
                    float cosTheta = Quaternion::DotProduct(current, rotation);

                    // Always take the smallest path to interpolate over.
                    if (cosTheta < 0.0f) {
                        rotation *= -1.0f;
                    }

                    // Apply rotation to this bone + all nested children bones.
                    boneVQSTransformations_[boneIndex].SetOrientation(rotation * current);

                    // Recalculate end effector position, taking into account the new transformed chain.
                    endEffectorPosition = GetGlobalTransformation(chain_[0]).GetTranslation(); // Model space.

                    distance = glm::length(endEffectorPosition - goalPosition);

                    if (distance < epsilon) {
                        return;
                    }
                }

                ++limit;
            }

            ++iterationCount;
        } while (distance > epsilon && iterationCount <= numIterations);
    }

    void Animator::SolveIKChainFABRIK(const glm::mat4& modelMatrix) {
        glm::mat4 modelInverse = glm::inverse(modelMatrix);
        const std::size_t numChainElements = chain_.size();

        GetIKChainLength();

        glm::vec3 chainOrigin = joints_[numChainElements - 1];// GetGlobalTransformation(chain_[numChainElements - 1]).GetTranslation(); // Model space.
        glm::vec3 goalPosition = modelInverse * glm::vec4(targetPosition_, 1.0f);

        const float epsilon = 0.000001f;
        const int numIterations = 200;

        for (unsigned int i = 0; i < numIterations; ++i) {
            glm::vec3 endEffectorPosition = joints_[0];

            if (glm::length(goalPosition - endEffectorPosition) < epsilon) {
                WorldToIKChain();
                return;
            }

            IterateBackwards(goalPosition);
            IterateForwards(chainOrigin);
        }

        WorldToIKChain();
    }

    void Animator::IK(const glm::mat4& modelMatrix) {
        if (chain_.empty() || !_target) {
            return;
        }

        SolveIKChainFABRIK(modelMatrix);
    }

    VQS Animator::GetGlobalTransformation(int index) {
        int startIndex = index;
        VQS out { };

        while (index != -1) {
            out = boneVQSTransformations_[index] * out;
            index = _target->_bones[index]._parentIndex;
        }

        return out;
    }

    void Animator::ApplyRotation(const Quaternion &rotation, int boneIndex) {
        // Apply rotation to this bone.
        VQS& transform = boneVQSTransformations_[boneIndex];
        transform.SetOrientation(rotation * transform.GetOrientation());
    }

    float Animator::GetIKChainLength() {
        const std::size_t chainLength = chain_.size();
        assert(chainLength > 1); // Invalid chain length.

        // Get joints in world space.
        joints_.clear();
        joints_.resize(chainLength);

        distances_.clear();
        distances_.resize(chainLength);
        distances_[0] = 0.0f;

        for (int i = (int)chainLength - 1; i >= 0; --i) {
            joints_[i] = GetGlobalTransformation(chain_[i]).GetTranslation(); // Model space.

            if (i < (int)chainLength - 1) {
                distances_[i] = glm::length(joints_[i] - joints_[i + 1]);
            }
        }

        return distances_[chainLength - 1];
    }

    void Animator::WorldToIKChain() {
        const std::size_t numChainElements = chain_.size();

        for (int i = (int)numChainElements - 1; i >= 1; --i) {
            int indexCurrent = i;
            int indexNext = i - 1;

            int jointIndexCurrent = chain_[indexCurrent];
            int jointIndexNext = chain_[indexNext];

            // Model space.
            VQS current = GetGlobalTransformation(jointIndexCurrent);
            VQS next = GetGlobalTransformation(jointIndexNext);

            glm::vec3 toNext = glm::normalize(next.GetTranslation() - current.GetTranslation());
            toNext = Quaternion::Invert(current.GetOrientation()) * toNext;

            // From model position before solving to solved model position.
            glm::vec3 toDesired = glm::normalize(joints_[indexNext] - current.GetTranslation());
            toDesired = Quaternion::Invert(current.GetOrientation()) * toDesired;

            Quaternion delta = glm::rotation(toNext, toDesired);

            // Always take the smallest path to interpolate over.
            if (Quaternion::DotProduct(boneVQSTransformations_[jointIndexCurrent].GetOrientation(), delta) < 0.0f) {
                delta *= -1.0f;
            }

            boneVQSTransformations_[jointIndexCurrent].SetOrientation(delta * boneVQSTransformations_[jointIndexCurrent].GetOrientation());
        }
    }

    void Animator::IterateBackwards(const glm::vec3 &goalPosition) {
        // Put the end effector at the goal position.
        joints_[0] = goalPosition;

        for (int i = 1; i < chain_.size(); ++i) {
            glm::vec3 direction = glm::normalize(joints_[i] - joints_[i - 1]); // Further away to root.
            joints_[i] = joints_[i - 1] + direction * distances_[i - 1];
        }
    }

    void Animator::IterateForwards(const glm::vec3 &rootPosition) {
        const std::size_t numChainElements = chain_.size();

        // Put the root of the IK chain at the original chain origin.
        joints_[numChainElements - 1] = rootPosition;

        for (int i = (int)numChainElements - 2; i >= 0; --i) {
            glm::vec3 direction = glm::normalize(joints_[i] - joints_[i + 1]);
            joints_[i] = joints_[i + 1] + direction * distances_[i];
        }
    }

}
