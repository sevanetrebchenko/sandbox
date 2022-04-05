
#version 450 core

in vec2 uv;
layout (location = 0) out vec4 fragColor;

uniform sampler2D inputTexture; // Explicit binding.
uniform float near;
uniform float far;

float remap(float depth) {
    return (2.0 * near) / (far + near - depth * (far - near));
}

void main() {
    fragColor = vec4(vec3(remap(texture(inputTexture, uv).r)), 1.0f);
}