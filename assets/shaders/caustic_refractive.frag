
// Renders refractive geometry (position, normal) to output buffers.

#version 450 core

layout (location = 0) out vec4 position;
layout (location = 1) out vec4 normal;

in vec4 worldPosition;
in vec4 worldNormal;

uniform float normalBlend; // Blend factor between vertex and face normals.

vec3 GetFaceNormal(vec3 pos) {
    vec3 dx = dFdx(pos);
    vec3 dy = dFdy(pos);
    return normalize(cross(dx, dy));
}

void main() {
    position = vec4(worldPosition.xyz, 1.0f);
    normal = vec4(mix(GetFaceNormal(worldPosition.xyz), normalize(worldNormal.xyz), normalBlend), 1.0f);
}
