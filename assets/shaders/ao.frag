
#version 450 core

#define PI 3.14159f

out vec4 fragColor;
in vec2 uvCoord;

uniform sampler2D position; // World-space positions.
uniform sampler2D normal;   // World-space normals.

uniform vec2 resolution;
uniform float aoRadius;
uniform float aoScale;
uniform float aoContrast;
uniform mat4 cameraTransform;

void main() {
    int xp = int(gl_FragCoord.x);
    int yp = int(gl_FragCoord.y);

    float x = float(xp) / resolution.x;
    float y = float(yp) / resolution.y;

    vec4 p = texture(position, uvCoord);
    vec4 n = texture(normal, uvCoord);

    float d = -(cameraTransform * p).z; //texture(depth, uvCoord).r; // Camera space depth.

    // Pseudo-random rotation.
    float phi = ((30 * xp) ^ yp) + (10 * xp * yp);

    // Generate random points.
    const int numPoints = 20;
    vec2 points[numPoints];

    for (int i = 0; i < numPoints; ++i) {
        float alpha = (float(i) + 0.5f) / float(numPoints);
        float h = (alpha * aoRadius) / d;
        float theta = (2.0f * PI) * alpha * (7.0f * float(numPoints) / 9.0f) + phi;

        points[i] = vec2(x + h * cos(theta), y + h * sin(theta));
    }

    // Calculate ambient occlusion.
    float s = 0.0f;
    float c = 0.1f * aoRadius;
    float delta = 0.001f;

    for (int i = 0; i < numPoints; ++i) {
        vec4 pi = texture(position, points[i]);

        vec4 wi = pi - p;
        float di = -(cameraTransform * vec4(pi.xyz, 1.0f)).z; //texture(depth, points[i]).r;

        // Heaviside step function.
        float H = ((aoRadius - length(wi)) < 0.0f) ? 0.0f : 1.0f;

        s += (max(0.0f, dot(n, wi) - delta * di) * H) / max(c * c, dot(wi, wi));
    }

    s *= (2.0f * PI) * c / float(numPoints);

    fragColor = vec4(vec3(max(0.0f, pow(1.0f - aoScale * s, aoContrast))), 1.0f);
}