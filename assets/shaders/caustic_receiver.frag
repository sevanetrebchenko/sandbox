
// Renders receiver geometry to a position buffer.

#version 450 core

out vec4 position;

in vec4 worldPosition;
in vec4 worldNormal;

void main() {
    position = vec4(worldPosition.xyz, 1.0f);
}
