
#include <framework/animator.h>
#include <framework/skinned_mesh.h>
#include <glm/gtx/quaternion.hpp>

namespace Sandbox {

    Animator::Animator() : _target(nullptr),
                           _selectedAnimation(nullptr),
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
                    _finalBoneTransformations[i] = glm::mat4(1.0f); // Identity.
                }
            }
        }
        else {
            // Loop animation forever.
            _currentTime += dt * _selectedAnimation->_speed * _playbackSpeed;
            _currentTime = std::fmod(_currentTime, _selectedAnimation->_duration);

            for (std::size_t i = 0; i < _target->_bones.size(); ++i) {
                Bone& bone = _target->_bones[i];
                int boneIndex = bone._index;

                InterpolateBone(boneIndex);

                // Compute final transformation for this bone.
                glm::mat4 parentTransform(1.0f);
                if (bone._parentIndex != -1) {
                    parentTransform = _finalBoneTransformations[bone._parentIndex];
                }

                // Apply parent transformation on top of bone local transformation.
                _finalBoneTransformations[boneIndex] = parentTransform * _finalBoneTransformations[boneIndex];
            }

            // Shift bone to proper position after calculating ALL bone matrices.
            for (Bone& bone : _target->_bones) {
                int boneIndex = bone._index;
                _finalBoneTransformations[boneIndex] = _finalBoneTransformations[boneIndex] * bone._boneToModel;
            }
        }

        _reset = _useBindPose;
    }

    void Animator::SetTarget(Skeleton *skeleton) {
        _target = skeleton;
        _finalBoneTransformations.resize(skeleton->_bones.size());
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
        // Compute interpolated (local) transformations for this bone.
        std::pair<KeyPosition, KeyPosition> positionKeys = _selectedAnimation->GetPositionKeyPair(boneIndex, _currentTime);
        std::pair<KeyRotation, KeyRotation> rotationKeys = _selectedAnimation->GetRotationKeyPair(boneIndex, _currentTime);
        std::pair<KeyScale, KeyScale> scaleKeys = _selectedAnimation->GetScaleKeyPair(boneIndex, _currentTime);

        // S * R * T
        glm::mat4 finalTransformation(1.0f);

        // Interpolate scale.
        {
            // Get time interval.
            const KeyScale& start = scaleKeys.first;
            const KeyScale& end = scaleKeys.second;

            float startTime = start.dt;
            float endTime = end.dt;

            // Normalize the distance between the two keyframes.
            float length = endTime - startTime;
            float current = _currentTime - startTime;
            float t = current / length;

            glm::mat4 scale = glm::scale(glm::mat4(1.0f), (1.0f - t) * start.scale + t * end.scale);
            finalTransformation = scale * finalTransformation;
        }

        // Interpolate rotation.
        {
            // Get time interval.
            const KeyRotation& start = rotationKeys.first;
            const KeyRotation& end = rotationKeys.second;

            float startTime = start.dt;
            float endTime = end.dt;

            // Normalize the distance between the two keyframes.
            float length = endTime - startTime;
            float current = _currentTime - startTime;
            float t = current / length;

            // Interpolate rotation.
            glm::mat4 rotation = glm::toMat4(glm::normalize(glm::slerp(start.orientation, end.orientation, t)));
            finalTransformation = rotation * finalTransformation;
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
            float current = _currentTime - startTime;
            float t = current / length;

            // Interpolate position.
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), (1.0f - t) * start.position + t * end.position);
            finalTransformation = translation * finalTransformation;
        }

        _finalBoneTransformations[boneIndex] = finalTransformation;
    }

    const std::vector<glm::mat4> &Animator::GetFinalBoneTransformations() const {
        return _finalBoneTransformations;
    }

}
