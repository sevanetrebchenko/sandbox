
#version 450 core

in vec2 textureCoordinates;

layout (binding = 0) uniform sampler2D inputTexture; // Explicit binding.

out vec4 fragColor;

void main() {
    fragColor = vec4(texture(inputTexture, textureCoordinates).rgb, 1.0f);
}