
#version 450 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 2) in vec2 uvCoordinates;

out vec2 uvCoord;

void main() {
    uvCoord = uvCoordinates;
    gl_Position = vec4(vertexPosition, 1.0f);
}
