
#version 450 core

layout(location = 0) out vec4 fragColor;

void main() {
     float depth = gl_FragCoord.z;
     fragColor = vec4(depth, depth * depth, depth * depth * depth, depth * depth * depth * depth);
}