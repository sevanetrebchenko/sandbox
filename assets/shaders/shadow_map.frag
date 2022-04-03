
#version 450 core

layout(location = 0) out vec4 fragColor;

in vec2 uv;
uniform sampler2D depthTexture;

void main() {
    float depth = texture(depthTexture, uv).r;
    fragColor = vec4(depth, depth * depth, depth * depth * depth, depth * depth * depth * depth);
}