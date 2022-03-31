
#version 450 core

in vec4 position;
layout(location = 0) out vec4 fragColor;

void main() {
     // Instead of storing depth z in a single channel shadow map, store (z, z ^ 2, z ^ 3, z ^ 4).
     float depth = position.w;
     fragColor = vec4(depth, depth * depth, depth * depth * depth, depth * depth * depth * depth);
}