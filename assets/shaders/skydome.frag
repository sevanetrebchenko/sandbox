
#version 450 core

#define PI 3.14159f

out vec4 fragColor;
in vec4 normal;

uniform sampler2D environmentMap;
uniform float exposure;
uniform float contrast;

vec2 NormalToSphereMapUV(vec3 n) {
    n = normalize(n);
    return vec2(0.5f - atan(n.z, n.x) / (2.0f * PI), acos(n.y) / PI);
}

void main(void) {
    vec4 color = texture(environmentMap, NormalToSphereMapUV(normal.xyz)).rgba;

    // Apply tone mapping.
    color.r = pow((exposure * color.r) / (exposure * color.r + 1.0f), contrast / 2.2f);
    color.g = pow((exposure * color.g) / (exposure * color.g + 1.0f), contrast / 2.2f);
    color.b = pow((exposure * color.b) / (exposure * color.b + 1.0f), contrast / 2.2f);

    fragColor = vec4(vec3(color), 1.0f);
}