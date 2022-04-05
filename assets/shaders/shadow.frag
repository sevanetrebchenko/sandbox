
#version 450 core

layout(location = 0) out vec4 fragColor;

in vec2 uv;
uniform float near;
uniform float far;

float remap(float depth) {
     return (2.0 * near) / (far + near - depth * (far - near));
}

void main() {
     float depth = remap(gl_FragCoord.z);

     // Four-channel shadow map.
     fragColor = vec4(depth, depth * depth, depth * depth * depth, depth * depth * depth * depth);
}
