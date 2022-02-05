
#version 450 core

layout (location = 0) out vec4 position;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 ambient;
layout (location = 3) out vec4 diffuse;
layout (location = 4) out vec4 specular;

in vec4 worldPosition;
in vec4 worldNormal;

uniform vec3 ambientCoefficient;
uniform vec3 diffuseCoefficient;
uniform vec3 specularCoefficient;
uniform float specularExponent;

uniform float timer;

vec3 GetFaceNormal(vec3 worldPosition) {
    vec3 dx = dFdx(worldPosition);
    vec3 dy = dFdy(worldPosition);
    return normalize(cross(dx, dy));
}

void main() {
    // Store fragment position in the first texture buffer.
    // position = vec4(viewPosition.xyz, 1.0f);
    position = vec4(worldPosition.xyz, 1.0f);

    // Store normal in the second texture buffer.
    // normal = vec4(viewNormal.xyz, 1.0f);
    normal = vec4(mix(GetFaceNormal(position.xyz), normalize(worldNormal.xyz), timer), 1.0f);//vec4(, 1.0f); // Color, not traditional normal value.

    // Store ambient color in the third texture buffer.
    ambient = vec4(ambientCoefficient, 1.0f);

    // Store diffuse color in the fourth texture buffer.
    diffuse = vec4(diffuseCoefficient, 1.0f);

    // Store specular color in the RBG channels and exponent in the A channel of the fifth texture buffer.
    specular = vec4(specularCoefficient, specularExponent);
}