
#version 450 core

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

// Shader outputs.
layout (location = 0) out vec4 fragColor;

void main(void) {
    vec2 uv = gl_FragCoord.xy / resolution;

    vec4 worldPosition = vec4(texture(position, uv).rgb, 1.0f);
    vec4 worldNormal = vec4(texture(normal, uv).rgb, 0.0f);
    vec3 ambientCoefficient = texture(ambient, uv).rgb;
    vec3 diffuseCoefficient = texture(diffuse, uv).rgb;
    vec3 specularCoefficient = texture(specular, uv).rgb;
    float specularExponent = texture(specular, uv).a;

    vec3 color = vec3(0.0f);
    float distanceToLight = distance(worldPosition.xyz, lightPosition);

    if (abs(distanceToLight) <= lightRadius) {
        // Calculate light contribution.
        vec4 N = normalize(worldNormal);
        vec4 viewVector = vec4(cameraPosition, 1.0f) - worldPosition;
        vec4 V = normalize(viewVector);

        // Light direction
        vec4 L = normalize(vec4(lightPosition, 1.0f) - worldPosition);

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

        // Attenuate light brightness based on the distance from the light position.
        color = (ambientComponent + diffuseComponent + specularComponent) * mix(lightBrightness, 0.0f, distanceToLight / lightRadius);
    }

    fragColor = vec4(color, 1.0f);
}
