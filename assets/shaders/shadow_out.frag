
#version 450 core

layout(location = 0) out vec4 fragColor;

in vec2 uv;
uniform sampler2D depthTexture;
uniform float cameraNearPlane;
uniform float cameraFarPlane;

float remap(float value, float oldMin, float oldMax, float newMin, float newMax) {
    return (value - oldMin) / (oldMax - oldMin) * (newMax - newMin) + newMin;
}

float remap(float depth) {
    return (2.0 * cameraNearPlane) / (cameraFarPlane + cameraNearPlane - depth * (cameraFarPlane - cameraNearPlane));
}

void main() {
    // Normalize depth to range [0.0, 1.0].
    float depth = remap(texture(depthTexture, uv).r, cameraNearPlane, cameraFarPlane, 0.0f, 1.0f);
//    float depth = remap(texture(depthTexture, uv).r);
    fragColor = vec4(vec3(depth), 1.0f);
}