
#version 450 core

uniform mat4 modelTransform;
uniform mat4 cameraTransform;

layout (location = 0) in vec3 vertexPosition;

void main() {
    gl_Position = cameraTransform * modelTransform * vec4(vertexPosition, 1.0);
}