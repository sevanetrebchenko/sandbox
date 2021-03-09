
#version 450 core

in vec2 textureCoordinates;

uniform sampler2D inputTexture; // Explicit binding.
uniform float cameraNearPlane;
uniform float cameraFarPlane;

layout(location = 0) out vec4 fragColor;

float LinearizeDepth(float depth) {
    return (2.0 * cameraNearPlane) / (cameraFarPlane + cameraNearPlane - depth * (cameraFarPlane - cameraNearPlane));
}

void main() {
    float depthValue = texture(inputTexture, textureCoordinates).r; // Depth texture has only one channel.
    fragColor = vec4(vec3(LinearizeDepth(depthValue)), 1.0f);
}