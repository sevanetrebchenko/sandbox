
#version 450 core

// Inputs from the vertex shader.
in vec2 uv;

// Texture samplers for various geometry buffers.
uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D ambient;
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D shadowMap;

// Camera information (in world space).
uniform vec3 cameraPosition;

// Global light information.
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform float lightBrightness;
uniform mat4 shadowTransform;
uniform float near;
uniform float far;

// Shader outputs.
layout (location = 0) out vec4 fragColor;

bool InRange(float value, float low, float high) {
    return value >= low && value <= high;
}

// -b +- sqrt(b ^ 2 - 4ac) / 2a
bool QuadraticFormula(float a, float b, float c, out float s1, out float s2) {
    float discriminant = (b * b) - 4.0f * a * c;
    if (discriminant < 0.0f) {
        // No real roots.
        return false;
    }

    float r1 = (-b - sqrt(discriminant)) / (2.0f * a);
    float r2 = (-b + sqrt(discriminant)) / (2.0f * a);

    s1 =  min(r1, r2);
    s2 = max(r1, r2);
    return true;
}

vec3 CholeskyDecomposition(float m11, float m12, float m13, float m22, float m23, float m33, float z1, float z2, float z3) {
    float a = sqrt(m11);
    float b = m12 / a;
    float c = m13 / a;
    float d = sqrt(m22 - (b * b));
    float e = (m23 - b * c) / d;
    float f = sqrt(m33 - (c * c) - (e * e));

    float c1 = z1 / a;
    float c2 = (z2 - b * c1) / d;
    float c3 = (z3 - c * c1 - e * c2) / f;

    float z = c3 / f;
    float y = (c2 - e * z) / d;
    float x = (c1 - b * y - c * z) / a;

    return vec3(x, y, z);
}

float remap(float depth) {
    return (2.0 * near) / (far + near - depth * (far - near));
}

// Returns the amount of shadowing present on point 'p'.
float G(vec4 p) {
    vec4 shadowCoordinate = shadowTransform * p; // Range: [0.0f, 1.0f] (UV space).
    shadowCoordinate /= shadowCoordinate.w;

    // Check 1: don't project fragments behind the position of the light.
    if (shadowCoordinate.w > 0.0f) {
        // Checks 2 - 5: don't process fragments outside the view frustum of the light.
        // uv = shadowCoordinate.xy
        if (InRange(shadowCoordinate.x, 0.0f, 1.0f) && InRange(shadowCoordinate.y, 0.0f, 1.0f)) {
            float zf = remap(shadowCoordinate.z);

            vec4 b = texture(shadowMap, shadowCoordinate.xy);

            float alpha = 0.001f;
            vec4 bp = (1.0f - alpha) * b + alpha * vec4(0.5f);

            vec3 c = CholeskyDecomposition(1.0f, bp[0], bp[1], bp[1], bp[2], bp[3], 1.0f, zf, zf * zf);

            // c[2] * z ^ 2 + c[1] * z + c[0] = 0 has solutions z2, z3 where z2 <= z3.
            float z2;
            float z3;
            if (!QuadraticFormula(c[2], c[1], c[0], z2, z3)) {
                return 0.0f;
            }

            // Adjust shadow bias based on the steepness of the surface angle to the light direction.
            float shadowBiasMin = 0.001f;
            float shadowBiasMax = 0.05f;
            float shadowBias = max(shadowBiasMax * (1.0f - dot(texture(normal, uv).rgb, normalize(-lightDirection))), shadowBiasMin);

            if (zf + shadowBias <= z2) {
                return 0.0f;
            }
            else if (zf + shadowBias <= z3) {
                float n = (zf * z3) - bp[0] * (zf + z3) + bp[1];
                float d = (z3 - z2) * (zf - z2);

                return n / d;
            }
            else {
                float n = (z2 * z3) - bp[0] * (z2 + z3) + bp[1];
                float d = (zf - z2) * (zf - z3);

                return 1.0f - n / d;
            }
        }
    }

    return 0.0f;
}

void main(void) {
    vec4 worldPosition = vec4(texture(position, uv).rgb, 1.0f);
    vec4 worldNormal = vec4(texture(normal, uv).rgb, 0.0f);
    vec3 ambientCoefficient = texture(ambient, uv).rgb;
    vec3 diffuseCoefficient = texture(diffuse, uv).rgb;
    vec3 specularCoefficient = texture(specular, uv).rgb;
    float specularExponent = texture(specular, uv).a;

    vec4 N = normalize(worldNormal);
    vec4 viewVector = vec4(cameraPosition, 1.0f) - worldPosition;
    vec4 V = normalize(viewVector);

    // Light direction
    vec4 L = vec4(normalize(-lightDirection), 0.0f);

    // Ambient.
    vec3 ambientComponent = lightColor * ambientCoefficient;

    // Diffuse.
    float diffuseMultiplier = max(dot(N, L), 0.0f);
    vec3 diffuseComponent = lightColor * diffuseCoefficient * diffuseMultiplier;

    // Specular
    vec3 specularComponent = vec3(0.0f);
    if (diffuseMultiplier > 0.0f) {
        // Blinn-Phong halfway vector.
        vec4 H = normalize(L + V);
        float specularMultiplier = max(dot(N, H), 0.0f);
        specularComponent = lightColor * specularCoefficient * pow(specularMultiplier, specularExponent);
    }

    vec3 color = ambientComponent + (1.0f - G(worldPosition)) * (diffuseComponent + specularComponent);
    fragColor = vec4(color * lightBrightness, 1.0f);
}
