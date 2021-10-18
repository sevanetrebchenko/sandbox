
#include <framework/animator.h>
#include <framework/skinned_mesh.h>
#include <glm/gtx/quaternion.hpp>
#include <framework/interpolation.h>

namespace Sandbox {

    Animator::Animator() : _target(nullptr),
                           _selectedAnimation(nullptr),
                           _keyInterpolationMethod(KeyInterpolationMethod::DEFAULT),
                           _quaternionInterpolationMethod(QuaternionInterpolationMethod::SLERP),
                           _currentTime(0.0f),
                           _playbackSpeed(1.0f),
                           _useBindPose(false),
                           _reset(false)
                           {
    }

    Animator::~Animator() = default;

    void Animator::Update(float dt) {
        if (!_target || !_selectedAnimation) {
            return;
        }

        if (_useBindPose) {
            // This needs to be done only once.
            if (!_reset) {
                // Need to reset bone positions to bind position.
                for (std::size_t i = 0; i < _target->_bones.size(); ++i) {
                    // Use identity VQS.
                    _finalBoneVQSTransformations[i] = VQS();
                }
            }
        }
        else {
            // Loop animation forever.
            _currentTime += dt * _selectedAnimation->_speed * _playbackSpeed;
            _currentTime = std::fmod(_currentTime, _selectedAnimation->_duration);

            for (Bone& bone : _target->_bones) {
                int boneIndex = bone._index;

                InterpolateBone(boneIndex);

                // Compute final transformation for this bone.
                VQS parentTransform { };
                if (bone._parentIndex != -1) {
                    parentTransform = _finalBoneVQSTransformations[bone._parentIndex];
                }

                // Apply parent transformation on top of bone local transformation.
                _finalBoneVQSTransformations[boneIndex] = parentTransform * _finalBoneVQSTransformations[boneIndex];
            }

            // Shift bone to proper position AFTER calculating ALL bone matrices.
            for (Bone& bone : _target->_bones) {
                int boneIndex = bone._index;
                _finalBoneVQSTransformations[boneIndex] = _finalBoneVQSTransformations[boneIndex] * bone._boneToModelVQS;
            }
        }

        _reset = _useBindPose;
    }

    void Animator::SetTarget(Skeleton *skeleton) {
        _target = skeleton;

        std::size_t numBones = skeleton->_bones.size();
        _finalBoneVQSTransformations.resize(numBones);
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

    const std::vector<VQS> &Animator::GetFinalBoneTransformations() const {
        return _finalBoneVQSTransformations;
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

            finalVQS.SetTranslation((1.0f - t) * start.position + t * end.position); // Lerp.
        }

        _finalBoneVQSTransformations[boneIndex] = finalVQS;
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

        _finalBoneVQSTransformations[boneIndex] = finalVQS;
    }

}
