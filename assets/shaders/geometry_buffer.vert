
#version 450 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexUV;

uniform mat4 cameraTransform;
uniform mat4 modelTransform;
uniform mat4 normalTransform;

out vec4 worldPosition;
out vec4 worldNormal;
out vec3 shadowPosition;

void main() {
    // Transform to screen coordinates (NDC).
//    viewNormal = transpose(inverse(viewTransform * modelTransform)) * vec4(vertexNormal, 0.0f);
//    viewPosition = viewTransform * modelTransform * vec4(vertexPosition, 1.0);

    worldNormal = normalTransform * vec4(vertexNormal, 0.0f);
    worldPosition = modelTransform * vec4(vertexPosition, 1.0);

    gl_Position = cameraTransform * modelTransform * vec4(vertexPosition, 1.0);
}