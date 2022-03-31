
#version 450 core

layout (location = 0) in vec3 vertexPosition;

uniform mat4 shadowTransform;
uniform mat4 modelTransform;

out vec4 position;

void main() {
    gl_Position = shadowTransform * modelTransform * vec4(vertexPosition, 1.0f);
    position = gl_Position;
}