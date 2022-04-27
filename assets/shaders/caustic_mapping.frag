
#version 450 core

out vec4 fragColor;

in vec3 n;
in vec3 l;

void main() {
    float radiance = 1.0f / (1.0f) * max(dot(n, l), 0.000001f);
    fragColor = vec4(vec3(radiance), 1.0f);
    // fragColor = vec4(1.0f, 1.0f, 1.0f, 0.2f);
}