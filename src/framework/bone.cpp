
#include <framework/bone.h>
#include <framework/assimp_helper.h>
#include <glm/gtx/quaternion.hpp>

namespace Sandbox {

//    // Reads in keys from the animation channel.
//    Bone::Bone(int boneID, std::string boneName, const aiNodeAnim *track) : _boneID(boneID),
//                                                                            _boneName(std::move(boneName))
//                                                                            {
//        // Position keys.
//        for (int i = 0; i < track->mNumPositionKeys; ++i) {
//            KeyPosition position { };
//            position.position = GetGLMVector(track->mPositionKeys[i].mValue);
//            position.dt = static_cast<float>(track->mPositionKeys[i].mTime);
//
//            _positions.emplace_back(position);
//        }
//
//        // Rotation keys.
//        for (int i = 0; i < track->mNumRotationKeys; ++i) {
//            KeyRotation rotation { };
//            rotation.orientation = GetGLMQuaternion(track->mRotationKeys[i].mValue);
//            rotation.dt = static_cast<float>(track->mRotationKeys[i].mTime);
//
//            _rotations.emplace_back(rotation);
//        }
//
//        // Scale keys.
//        for (int i = 0; i < track->mNumScalingKeys; ++i) {
//            KeyScale scale { };
//            scale.scale = GetGLMVector(track->mScalingKeys[i].mValue);
//            scale.dt = static_cast<float>(track->mScalingKeys[i].mTime);
//
//            _scales.emplace_back(scale);
//        }
//    }
//
//    Bone::~Bone() {
//    }
//
//    void Bone::Update(float dt) {
//        // Interpolate all the channels and stores updated matrix in _localTransform.
//        glm::mat4 translation = InterpolatePosition(dt);
//        glm::mat4 rotation = InterpolateRotation(dt);
//        glm::mat4 scale = InterpolateScale(dt);
//
//        _localTransform = translation * rotation * scale;
//    }
//
//    const glm::mat4 &Bone::GetLocalTransform() const {
//        return _localTransform;
//    }
//
//    const std::string &Bone::GetName() const {
//        return _boneName;
//    }
//
//    int Bone::GetBoneID() const {
//        return _boneID;
//    }
//
//    float Bone::Lerp(float start, float end, float duration) {
//        // Normalize to [0, 1].
//        start /= duration;
//        end /= duration;
//        duration = 1.0f;
//
//        return start + duration * (end - start);
//    }
//
//    float Bone::Slerp(float start, float end, float duration) {
//        // Normalize to [0, 1].
//        start /= duration;
//        end /= duration;
//        duration = 1.0f;
//
//        return 0.0f;
//    }
//
//    float Bone::GetFactor(float begin, float end, float duration) {
//        float midWayLength = duration - begin;
//        float framesDiff = end - begin;
//        return midWayLength / framesDiff;
//    }
//
//    glm::mat4 Bone::InterpolatePosition(float dt) {
//        // Position.
//        if (_positions.size() == 1) {
//            return glm::translate(glm::mat4(1.0f), _positions[0].position);
//        }
//
//        // Get index of position track.
//        int startPositionIndex = -1;
//        for (int i = 0; i < _positions.size() - 1; ++i) {
//            if (dt < _positions[i + 1].dt) {
//                startPositionIndex = i;
//                break;
//            }
//        }
//        assert(startPositionIndex != -1); // Reached past end of animation.
//        int endPositionIndex = startPositionIndex + 1;
//
//        float positionScaleFactor = GetFactor(_positions[startPositionIndex].dt, _positions[endPositionIndex].dt, dt);
//        return glm::translate(glm::mat4(1.0f), glm::mix(_positions[startPositionIndex].position, _positions[endPositionIndex].position, positionScaleFactor));
//    }
//
//    glm::mat4 Bone::InterpolateRotation(float dt) {
//        if (_rotations.size() == 1) {
//            return glm::toMat4(glm::normalize(_rotations[0].orientation));
//        }
//
//        // Get index of rotation track.
//        int startRotationIndex = -1;
//        for (int i = 0; i < _rotations.size() - 1; ++i) {
//            if (dt < _rotations[i + 1].dt) {
//                startRotationIndex = i;
//                break;
//            }
//        }
//        assert(startRotationIndex != -1);
//        int endRotationIndex = startRotationIndex + 1;
//
//        float rotationScaleFactor = GetFactor(_rotations[startRotationIndex].dt, _rotations[endRotationIndex].dt, dt);
//        glm::quat rotation = glm::normalize(glm::slerp(_rotations[startRotationIndex].orientation, _rotations[endRotationIndex].orientation, rotationScaleFactor));
//
//        return glm::toMat4(rotation);
//    }
//
//    glm::mat4 Bone::InterpolateScale(float dt) {
//        if (_scales.size() == 1) {
//            return glm::scale(glm::mat4(1.0f), _scales[0].scale);
//        }
//
//        // Get index of scale track.
//        int startScaleIndex = -1;
//        for (int i = 0; i < _scales.size() - 1; ++i) {
//            if (dt < _scales[i + 1].dt) {
//                startScaleIndex = i;
//                break;
//            }
//        }
//        assert(startScaleIndex != -1);
//        int endScaleIndex = startScaleIndex + 1;
//
//        float scaleScaleFactor = GetFactor(_scales[startScaleIndex].dt, _scales[endScaleIndex].dt, dt);
//        return glm::scale(glm::mat4(1.0f), glm::mix(_scales[startScaleIndex].scale, _scales[endScaleIndex].scale, scaleScaleFactor));
//    }
}