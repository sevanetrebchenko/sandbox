
#version 450 core

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 cameraTransform;
uniform mat4 normalTransform;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 boneTransforms[MAX_BONES];

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoordinate;
layout (location = 3) in vec4 boneIDs;
layout (location = 4) in vec4 boneWeights;

out vec4 viewPosition;
out vec4 viewNormal;

void main()
{
    vec4 finalPosition = vec4(0.0f);
    vec4 finalNormal = vec4(0.0f);

    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
        if (boneIDs[i] == -1) {
            continue;
        }

        if (boneIDs[i] >= MAX_BONES) {
            finalPosition = vec4(vertexPosition, 0.0f);
            finalNormal = vec4(vertexNormal, 1.0f);
            break;
        }

        int ID = int(boneIDs[i]);

        mat4 transform = boneTransforms[ID];

        vec4 localPosition = transform * vec4(vertexPosition, 1.0f);
        finalPosition += localPosition * boneWeights[i];

        vec4 localNormal = vec4(mat3(transform) * vertexNormal, 0.0f);
        finalNormal += localNormal;// * boneWeights[i];
    }

    // Transform to clip space
    viewNormal = viewTransform * normalTransform * vec4(vec3(finalNormal), 0.0f);
    viewPosition = viewTransform * modelTransform * vec4(vec3(finalPosition), 1.0f);

    // Camera transform contains both view and projection.
    gl_Position = cameraTransform * modelTransform * vec4(vec3(finalPosition), 1.0f);
}