
#version 450 core

#define PI 3.14159f

out vec4 fragColor;

// Texture samplers for various geometry buffers.
uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D ambient;
uniform sampler2D diffuse;
uniform sampler2D specular;

uniform vec2 resolution;

// Camera information (in world space).
uniform vec3 cameraPosition;

// Local light information (per light).
uniform vec3 lightPosition;
uniform float lightRadius;
uniform vec3 lightColor;
uniform float lightBrightness;

// BRDF.
uniform int model; // 0 for Phong, 1 for GGX, 2 for Beckman.
#define PHONG   0
#define GGX     1
#define BECKMAN 2
uniform float exposure;
uniform float contrast;

vec2 uvCoord;
float epsilon = 0.0001f;

// D - microfacet distribution.
// In the Phong BRDF model, 'alpha' is valid in the range [0.0, infinity], with higher values representing a smoother surface.
// In both the GGX and Beckman models, 'alpha' is valid in the range [0.0, 1.0], with higher values representing a rougher surface.
float D(vec3 H) {
    vec3 N = normalize(texture(normal, uvCoord).xyz);
    float alpha = texture(specular, uvCoord).a;
    float error = 0.0f; // TODO: what is the error term?

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

void main(void) {
    uvCoord = gl_FragCoord.xy / resolution;
    vec4 p = texture(position, uvCoord);

    vec3 N = normalize(texture(normal, uvCoord).xyz);
    vec3 V = normalize(cameraPosition - p.xyz);
    vec3 L = normalize(lightPosition - p.xyz);
    vec3 H = normalize(L + V);

    vec3 color = vec3(0.0f);
    float distanceToLight = distance(p.xyz, lightPosition);

    if (abs(distanceToLight) <= lightRadius) {
        // Point lights give off no ambient light.
        // vec3 Ka = texture(ambient, uvCoord).rgb;
        vec3 ambient = vec3(0.0f);

        // Diffuse.
        vec3 Kd = texture(diffuse, uvCoord).rgb;
        vec3 diffuse = Kd / PI;

        // Specular.
        vec3 specular = vec3(0.0f);
        float NdotL = dot(N, L);
        if (NdotL > 0.0f) {
            // Components of BRDF model.
            float roughness = D(H);
            vec3 fresnel = F(L, H);
            float occlusion = G(L, H) * G(V, H);// Calculated using Smith form.

            specular = (roughness * fresnel * occlusion) / (4.0f * NdotL * dot(V, N));
        }

        // Attenuate light brightness based on the distance from the light position.
        vec3 I = lightColor * mix(lightBrightness, 0.0f, distanceToLight / lightRadius);

        // Shadow for point lights requires a different shadow map.
        // float shadow = (1.0f - G(p));

        color = ambient + (I * max(NdotL, 0.0f) * (diffuse + specular));

        // Tone mapping.
        color.r = pow((exposure * color.r) / (exposure * color.r + 1.0f), contrast / 2.2f);
        color.g = pow((exposure * color.g) / (exposure * color.g + 1.0f), contrast / 2.2f);
        color.b = pow((exposure * color.b) / (exposure * color.b + 1.0f), contrast / 2.2f);
    }

    fragColor = vec4(color, 1.0f);
}
