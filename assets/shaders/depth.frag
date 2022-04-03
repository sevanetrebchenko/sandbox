
#version 450 core

in vec2 uv;

uniform sampler2D inputTexture; // Explicit binding.
uniform float cameraNearPlane;
uniform float cameraFarPlane;

out vec4 fragColor;

void main() {
    // Convert from depth buffer range to [0.0, 1.0].
    float depth = (texture(inputTexture, uv).r - cameraNearPlane) / (cameraFarPlane - cameraNearPlane);
    fragColor = vec4(vec3(texture(inputTexture, uv).r), 1.0f);
}