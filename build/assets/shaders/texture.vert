
#version 450 core

uniform mat4 modelTransform;
uniform mat4 cameraTransform;

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 normalPosition;
layout (location = 2) in vec2 uvCoordinate;

out vec2 uv;

void main() {
    uv = uvCoordinate;
    gl_Position = cameraTransform * modelTransform * vec4(vertexPosition, 1.0);
}