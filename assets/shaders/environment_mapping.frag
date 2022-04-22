
#version 450 core

out vec4 fragColor;

in vec4 worldPosition;
in vec4 worldNormal;

// Skybox samplers (one for each side).
uniform sampler2D posX;
uniform sampler2D negX;
uniform sampler2D posY;
uniform sampler2D negY;
uniform sampler2D posZ;
uniform sampler2D negZ;

uniform float ior;
uniform vec3 cameraPosition;

// Samples cube map composed of 6 faces based on the provided direction vector 'd'.
// https://en.wikipedia.org/wiki/Cube_mapping
vec3 SampleCubemap(vec3 d) {
    float x = d.x;
    float y = d.y;
    float z = d.z;

    float absX = abs(x);
    float absY = abs(y);
    float absZ = abs(z);

    float maxAxis;
    float uc;
    float vc;
    int index = 0;

    if (absX >= absY && absX >= absZ) {
        if (x > 0) {
            maxAxis = absX;
            uc = z;
            vc = y;
            index = 1;
        }
        else {
            maxAxis = absX;
            uc = -z;
            vc = y;
            index = 0;
        }
    }
    if (absY >= absX && absY >= absZ) {
        if (y > 0) {
            maxAxis = absY;
            uc = -x;
            vc = -z;
            index = 3;
        }
        else {
            maxAxis = absY;
            uc = -x;
            vc = z;
            index = 2;
        }

    }
    if (absZ >= absX && absZ >= absY) {
        if (z > 0) {
            maxAxis = absZ;
            uc = -x;
            vc = y;
            index = 5;
        }
        else {
            maxAxis = absZ;
            uc = x;
            vc = y;
            index = 4;
        }
    }

    // Convert range from -1 to 1 to 0 to 1
    float u = (uc / maxAxis + 1.0f) * 0.5f;
    float v = (vc / maxAxis + 1.0f) * 0.5f;

    switch (index) {
        case 0:
            return texture(negX, vec2(u, v)).rgb;
        case 1:
            return texture(posX, vec2(u, v)).rgb;
        case 2:
            return texture(negY, vec2(u, v)).rgb;
        case 3:
            return texture(posY, vec2(u, v)).rgb;
        case 4:
            return texture(negZ, vec2(u, v)).rgb;
        case 5:
            return texture(posZ, vec2(u, v)).rgb;

        default:
            return texture(negX, vec2(u, v)).rgb;
    }
}

void main() {
    vec3 R = refract(normalize(worldPosition.xyz - cameraPosition), normalize(worldNormal.xyz), 1.0f / ior);
    fragColor = vec4(SampleCubemap(normalize(R)), 1.0f);
}