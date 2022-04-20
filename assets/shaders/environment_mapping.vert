
#version 450 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 uvCoordinates;

uniform mat4 modelTransform;
uniform mat4 normalTransform;
uniform mat4 cameraTransform;

out vec4 worldPosition;
out vec4 worldNormal;

void main() {
    worldPosition = modelTransform * vec4(vertexPosition, 1.0f);
    worldNormal = normalTransform * vec4(vertexNormal, 0.0f);

    gl_Position = cameraTransform * worldPosition;
}