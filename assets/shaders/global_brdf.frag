
#version 450 core

#define PI 3.14159f
#define NUM_RANDOM_POINTS 40

in vec2 uvCoord;
out vec4 fragColor;

// Samplers from geometry pass.
uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D ambient;
uniform sampler2D diffuse;
uniform sampler2D specular;

// Samplers from shadow mapping pass.
uniform sampler2D shadowMap;

// Samplers for BRDF / Environment Lighting.
uniform sampler2D environmentMap;
uniform sampler2D irradianceMap;
uniform int model; // 0 for Phong, 1 for GGX, 2 for Beckman.
#define PHONG   0
#define GGX     1
#define BECKMAN 2
uniform float exposure;
uniform float contrast;

uniform vec3 cameraPosition;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform float lightBrightness;
uniform mat4 shadowTransform;
uniform float near;
uniform float far;

layout (std140, binding = 4) uniform HammersleyDistribution {
    int count;
    vec2 points[NUM_RANDOM_POINTS];
} hammersley;

float epsilon = 0.0001f;

bool InRange(float value, float low, float high) {
    return value >= low && value <= high;
}

// Section: Moment Shadow Mapping:
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
float Shadow(vec4 p) {
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
            vec4 bp = (1.0f - alpha) * b + alpha * vec4(0.8f);

            vec3 c = CholeskyDecomposition(1.0f, bp[0], bp[1], bp[1], bp[2], bp[3], 1.0f, zf, zf * zf);

            // c[2] * z ^ 2 + c[1] * z + c[0] = 0 has solutions z2, z3 where z2 <= z3.
            float z2;
            float z3;
            if (!QuadraticFormula(c[2], c[1], c[0], z2, z3)) {
                return 0.0f;
            }

            // Adjust shadow bias based on the steepness of the surface angle to the light direction.
            float shadowBiasMin = 0.001f;
            float shadowBiasMax = 0.005f;
            float shadowBias = max(shadowBiasMax * (1.0f - dot(normalize(texture(normal, uvCoord)).rgb, normalize(-lightDirection))), shadowBiasMin);

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

// Section: Physically Based Lighting.

// Converter between Phong and GGX/Beckman alpha values.
float FromPhong(float alpha) {
    return sqrt(2.0f / (alpha + 2.0f));
}

float ToPhong(float alpha) {
    return -2.0f + 2.0f / (alpha * alpha);
}

// D - microfacet distribution.
// In the Phong BRDF model, 'alpha' is valid in the range [0.0, infinity], with higher values representing a smoother surface.
// In both the GGX and Beckman models, 'alpha' is valid in the range [0.0, 1.0], with higher values representing a rougher surface.
float D(vec3 H) {
    vec3 N = normalize(texture(normal, uvCoord).xyz);
    float alpha = texture(specular, uvCoord).a;
    float error = 1.0f; // TODO: what is the error term?

    float hn = dot(H, N);
    if (hn > 1.0f) {
        return error;
    }

    float t = sqrt(1.0f - hn * hn) / hn;
    if (abs(t) < epsilon) {
        return error;
    }

    switch (model) {
        case PHONG: {
            return (alpha + 2.0f) / (2.0f * PI) * pow(hn, alpha);
        }
        case GGX: {
            float d = (alpha * alpha + t * t);
            return (alpha * alpha) / (PI * pow(hn, 4) * (d * d));
        }
        case BECKMAN: {
            return 1.0f / (PI * (alpha * alpha) * pow(hn, 4)) * exp(-(t * t) / (alpha * alpha));
        }
        default: {
            return error;
        }
    }
}

// F - Fresnel (reflection).
vec3 F(vec3 L, vec3 H) {
    vec3 Ks = texture(specular, uvCoord).xyz;
    return Ks + (1.0f - Ks) * pow(1.0f - max(dot(L, H), 0.0f), 5.0f);
}

// G - visibility
// Accounts for microfacet geometry occlusion and self-shadowing.
float G(vec3 v, vec3 H) {
    // Realtime approximation:
    // return 1.0f / (dot(L, H) * dot(L, H));

    vec3 N = normalize(texture(normal, uvCoord).xyz);
    float alpha = texture(specular, uvCoord).a;
    float error = 0.0f;

    float vn = dot(v, N);
    if (vn > 1.0f) {
        return error;
    }

    float t = sqrt(1.0f - vn * vn) / vn;
    if (abs(t) < epsilon) {
        return error;
    }

    switch (model) {
        case PHONG: {
            float a = sqrt(alpha / 2.0f + 1.0f) / t;
            if (a < 1.6f) {
                return (3.535f * a + 2.181f * (a * a)) / (1.0f + 2.276f * a + 2.577f * (a * a));
            }
            else {
                return 1.0f;
            }
        }
        case GGX: {
            return 2.0f / (1.0f + sqrt(1.0f + (alpha * alpha) * (t * t)));
        }
        case BECKMAN: {
            float a = 1.0f / (alpha * t);
            if (a < 1.6f) {
                return (3.535f * a + 2.181f * (a * a)) / (1.0f + 2.276f * a + 2.577f * (a * a));
            }
            else {
                return 1.0f;
            }
        }
        default: {
            return error;
        }
    }
}

float T(float xi) {
    float alpha = texture(specular, uvCoord).a;

    switch (model) {
        case PHONG: {
            return acos(pow(xi, 1.0f / (alpha + 1.0f)));
        }
        case GGX: {
            return atan((alpha * sqrt(xi)) / sqrt(1.0f - xi));
        }
        case BECKMAN: {
            return atan(sqrt(-(alpha * alpha) * log(1.0f - xi)));
        }
        default: {
            return 0.0f;
        }
    }
}

// Section: Image Based Lighting
vec2 NormalToSphereMapUV(vec3 n) {
    n = normalize(n);
    return vec2(atan(n.z, n.x) / (2.0f * PI), acos(n.y) / PI);

    float theta = atan(n.z, n.x);
    float r = length(n);
    float phi = acos(n.y / r);
    return vec2(theta / (2.0f * PI), phi / PI);
}

vec3 SphereMapUVToNormal(vec2 uv) {
    float u = uv.x;
    float v = uv.y;
    return normalize(vec3(cos((2.0f * PI) * (0.5f - u)) * sin(PI * v), sin((2.0f * PI) * (0.5f - u)) * sin(PI * v), cos(PI * v)));
}

vec3 EnvironmentSpecular(vec3 N, vec3 V) {
    int count = hammersley.count;

    // Build orthonormal basis around the reflection direction.
    vec3 R = 2.0f * dot(N, V) * N - V;
    vec3 A = normalize(vec3(-R.z, 0, R.x)); // normalize(cross(vec3(0.0f, 1.0f, 0.0f), R));
    vec3 B = normalize(cross(R, A));

    ivec2 dimensions = textureSize(environmentMap, 0);
    vec3 specular = vec3(0.0f);

    float p = 0.5f * log2(float(dimensions.x * dimensions.y) / float(count));

    for (int i = 0; i < count; ++i) {
        // Generate pseudo-random specular reflection direction using Hammersley distribution.
        vec2 random = hammersley.points[i];

        vec3 L = SphereMapUVToNormal(vec2(random.x, T(random.y) / PI));
        L = normalize((L.x * A) + (L.y * B) + (L.z * R)); // Rotated direction 'wk'.
        vec3 H = normalize(L + V);

        //        float theta = T(random.x);
        //        float phi = random.y * (2.0f * PI);
        //        vec3 L = normalize((sin(theta) * cos(phi) * A) + (sin(theta) * sin(phi) * B) + (cos(theta) * R)); // Rotated direction 'wk'.
        //        vec3 H = normalize(L + V);

        // https://developer.nvidia.com/gpugems/gpugems3/part-iii-rendering/chapter-20-gpu-based-importance-sampling
        // Averaging all pixels within the solid angle can be approximated by choosing an appropriate mipmap level.
        float level = p - 0.5f * log2(D(H) / 4.0f);
        vec3 Li = textureLod(environmentMap, NormalToSphereMapUV(L), level).rgb;

        float NdotL = dot(N, L);

        if (NdotL > 0.0f) {
            // Components of BRDF model.
            // D(H) component is canceled out by careful selection of sampled directions (importance sampling).
            // vec3 roughness = D(H);
            vec3 fresnel = F(L, H);
            float occlusion = G(L, H) * G(V, H); // Calculated using Smith form.

            specular += (fresnel * occlusion) / (4.0f * NdotL * dot(V, N)) * Li * NdotL;
        }
    }

    return specular / float(count);
}

void main(void) {
    vec4 p = texture(position, uvCoord);

    vec3 N = normalize(texture(normal, uvCoord).xyz);
    vec3 V = normalize(cameraPosition - p.xyz);

    // Ambient.
    vec3 Ka = texture(ambient, uvCoord).rgb;
    vec3 ambientComponent = Ka;

    // Diffuse.
    vec3 Kd = texture(diffuse, uvCoord).rgb;
    vec3 diffuseComponent = (Kd / PI) * texture(irradianceMap, NormalToSphereMapUV(N)).rgb;

    // Specular.
    vec3 Ks = texture(specular, uvCoord).xyz;
    vec3 specularComponent = Ks * EnvironmentSpecular(N, V);

    vec3 L = normalize(-lightDirection); // Directional light.
    vec3 H = normalize(L + V);
    vec3 Li = lightColor * lightBrightness;
    float shadowComponent = 1.0f;// TODO: (1.0f - Shadow(p)) * max(dot(N, L), 0.0f);

    vec3 color = ambientComponent + shadowComponent * Li * (diffuseComponent + specularComponent);

    // Tone mapping.
    color = pow((exposure * color) / (exposure * color + 1.0f), vec3(contrast) / 2.2f);
    fragColor = vec4(color, 1.0f);
}


// Pure PBL (no IBL).
//    vec4 p = texture(position, uvCoord);
//
//    vec3 N = normalize(texture(normal, uvCoord).xyz);
//    vec3 V = normalize(cameraPosition - p.xyz);
//    vec3 L = normalize(-lightDirection); // Directional light.
//    vec3 H = normalize(L + V);
//
//    // Ambient.
//    vec3 Ka = texture(ambient, uvCoord).rgb;
//    vec3 ambient = Ka;
//
//    // Diffuse.
//    vec3 Kd = texture(diffuse, uvCoord).rgb;
//    vec3 diffuse = Kd / PI;
//
//    // Specular.
//    vec3 specular = vec3(0.0f);
//    if (dot(N, L) > 0.0f) {
//        // Components of BRDF model.
//        float roughness = D(H);
//        vec3 fresnel = F(L, H);
//        float occlusion = G(L, H) * G(V, H); // Calculated using Smith form.
//
//        specular = (roughness * fresnel * occlusion) / (4.0f * dot(N, L) * dot(V, N));
//    }
//
//    vec3 I = lightColor * lightBrightness;
//    float shadow = (1.0f - G(p));
//
//    vec3 color = ambient + shadow * (I * max(dot(N, L), 0.0f) * (diffuse + specular));
//    fragColor = vec4(color, 1.0f);