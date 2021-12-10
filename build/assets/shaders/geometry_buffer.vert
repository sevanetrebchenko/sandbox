
#version 450 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;

uniform mat4 cameraTransform;
uniform mat4 viewTransform;
uniform mat4 modelTransform;

out vec4 viewPosition;
out vec4 viewNormal;

void main() {
    // Transform to clip space
    viewNormal = transpose(inverse(viewTransform * modelTransform)) * vec4(vertexNormal, 0.0f);
    viewPosition = viewTransform * modelTransform * vec4(vertexPosition, 1.0);

    gl_Position = cameraTransform * modelTransform * vec4(vertexPosition, 1.0);
}