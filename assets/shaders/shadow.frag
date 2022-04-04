
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
     float depth = texture(depthTexture, uv).r;
     //    float depth = remap(texture(depthTexture, uv).r, cameraNearPlane, cameraFarPlane, 0.0f, 1.0f);
     //    float depth = remap(texture(depthTexture, uv).r);

     // 4 channel shadow map.
     fragColor = vec4(depth, depth * depth, depth * depth * depth, depth * depth * depth * depth);
}