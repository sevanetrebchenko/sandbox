
#version 450 core

layout (location = 0) out vec4 fragColor;

in vec2 uv;
uniform sampler2D inputTexture; // Explicit binding.

void main() {
    fragColor = vec4(texture(inputTexture, uv).rgb, 1.0f);
}