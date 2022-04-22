
#version 450 core

uniform sampler2D receiver;
uniform sampler2D refractivePositions;
uniform sampler2D refractiveNormals;

uniform mat4 cameraTransform;
uniform mat4 viewTransform;
uniform mat4 modelTransform;
uniform mat4 normalTransform;
uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform float ior;

out vec3 n;
out vec3 l;

vec3 EstimateIntersection(vec3 v, vec3 r) {
    const int steps = 10;

    vec3 p = v + 1.0f * r;
    vec4 texPt;
    vec3 recPos;
    vec2 tc;

    for (int i = 0; i < steps; ++i) {
        texPt = viewTransform * vec4(p, 1.0f);
        tc = vec2(0.5f * (texPt.xy / texPt.w) + vec2(0.5f, 0.5f));

        recPos = texture(receiver, tc).xyz;
        p = v + distance(v, recPos) * r;
    }

    return texture(receiver, tc).xyz;
}

void main() {
    float u = (gl_VertexID % 128) / float(128);
    float v = (gl_VertexID / float(128)) / float(128);
    vec2 uv = vec2(u, v);

    vec3 worldPosition = texture(refractivePositions, uv).rgb;
    vec3 worldNormal = texture(refractiveNormals, uv).rgb;

    vec3 R = refract(normalize(lightPosition - worldPosition), normalize(worldNormal), 1.0f / ior);
    vec3 P = EstimateIntersection(worldPosition, normalize(R));

    n = normalize(worldNormal);
    l = normalize(lightPosition - worldPosition);

    gl_Position = cameraTransform * vec4(P, 1.0f);
}