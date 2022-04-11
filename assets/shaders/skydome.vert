
#version 450 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;

uniform mat4 cameraTransform;
uniform mat4 modelTransform;
uniform mat4 normalTransform;

out vec4 normal;

void main() {
    gl_Position = cameraTransform * modelTransform * vec4(vertexPosition, 1.0f);

    // Compute world normal.
    normal = normalTransform * vec4(vertexNormal, 0.0f);
}