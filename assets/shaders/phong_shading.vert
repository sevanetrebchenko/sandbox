
#version 450 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoordinate;
layout (location = 3) in vec4 boneIDs;
layout (location = 4) in vec4 boneWeights;

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 cameraTransform;
uniform mat4 normalTransform;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

struct VQS {
    vec3 translation;
    vec4 rotation; // Quaternion.
    float scale;
};

uniform int numBones;
uniform VQS[MAX_BONES] finalBoneTransformations;

out vec4 viewPosition;
out vec4 viewNormal;

vec3 ApplyQuaternion(in vec4 quaternion, in vec3 position) {
    float s = quaternion.w;
    vec3 v = quaternion.xyz;
    return ((2.0f * s * s - 1.0f) * position) + (2.0f * dot(v, position) * v) + (2.0f * s * cross(v, position));
}

vec4 InverseQuaternion(in vec4 quaternion) {
    float s = quaternion.w;
    vec3 v = quaternion.xyz;

    float inverseLengthSquared = (s * s) + dot(v, v);
    return vec4(s * inverseLengthSquared, v * -inverseLengthSquared);
}

vec3 TransformWithVQS(in VQS vqs, in vec3 position) {
    // r' = q (sr) q^-1 + v

    // Apply quaternion.
    position = ApplyQuaternion(vqs.rotation, position);

    // Apply translation.
    position += vqs.translation;

    return position;
}

void main() {
    vec4 finalPosition = vec4(0.0f);
    vec4 finalNormal = vec4(0.0f);

    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
        if (boneIDs[i] == -1) {
            continue;
        }

        if (boneIDs[i] >= numBones) {
            finalPosition = vec4(vertexPosition, 0.0f);
            finalNormal = vec4(vertexNormal, 1.0f);
            break;
        }

        int ID = int(boneIDs[i]);

        VQS vqs = finalBoneTransformations[ID];

        vec4 localPosition = vec4(TransformWithVQS(vqs, vertexPosition), 1.0f);
        finalPosition += localPosition * boneWeights[i];

        vec4 localNormal = vec4(TransformWithVQS(vqs, vertexNormal), 0.0f);
        finalNormal += localNormal;
    }

    // Transform to clip space
    viewNormal = viewTransform * normalTransform * vec4(vec3(finalNormal), 0.0f);
    viewPosition = viewTransform * modelTransform * vec4(vec3(finalPosition), 1.0f);

    // Camera transform contains both view and projection.
    gl_Position = cameraTransform * modelTransform * vec4(vec3(finalPosition), 1.0f);
}