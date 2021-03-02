
#version 450 core

in vec2 uv;
uniform sampler2D modelTexture;

// Shader outputs.
layout(location = 0) out vec4 fragColor;

void main(void) {
    fragColor = vec4(texture(modelTexture, uv).rgb, 1.0f);
}