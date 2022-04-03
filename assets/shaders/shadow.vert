
#version 450 core

layout (location = 0) in vec3 vertexPosition;

uniform mat4 shadowTransform;
uniform mat4 modelTransform;

void main() {
    gl_Position = shadowTransform * modelTransform * vec4(vertexPosition, 1.0f); // NDC.
}