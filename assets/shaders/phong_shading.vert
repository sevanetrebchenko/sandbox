
#version 450 core

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 cameraTransform;
uniform mat4 normalTransform;

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoordinate;

out vec4 viewPosition;
out vec4 viewNormal;

void main()
{
    // Transform to screen coodinates (NDC).
    viewPosition = cameraTransform * modelTransform * vec4(vertexPosition, 1.0);
    viewNormal = cameraTransform * normalTransform * vec4(vertexNormal, 0.0f);

    gl_Position = cameraTransform * modelTransform * vec4(vertexPosition, 1.0);
}