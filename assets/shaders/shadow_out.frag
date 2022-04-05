
#version 450 core

layout(location = 0) out vec4 fragColor;

in vec2 uv;
uniform sampler2D depthTexture;
uniform float near;
uniform float far;

float remap(float depth) {
    return (2.0 * near) / (far + near - depth * (far - near));
}

void main() {
    fragColor = vec4(vec3(remap(texture(depthTexture, uv).r)), 1.0f);
}