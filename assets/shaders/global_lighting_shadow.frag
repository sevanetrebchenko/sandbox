
#version 450 core

// Inputs from the vertex shader.
in vec2 textureCoordinates; // TODO: maybe use glFragCoord?

// Texture samplers for various geometry buffers.
uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D ambient;
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D shadow;

// Camera information (in world space).
uniform vec3 cameraPosition;

// Global light information.
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform float lightBrightness;

uniform mat4 shadowTransform;

// Shader outputs.
layout (location = 0) out vec4 fragColor;

bool InRange(float value, float low, float high) {
    return value >= low && value <= high;
}

void main(void) {
    vec4 worldPosition = vec4(texture(position, textureCoordinates).rgb, 1.0f);
    vec4 worldNormal = vec4(texture(normal, textureCoordinates).rgb, 0.0f);
    vec3 ambientCoefficient = texture(ambient, textureCoordinates).rgb;
    vec3 diffuseCoefficient = texture(diffuse, textureCoordinates).rgb;
    vec3 specularCoefficient = texture(specular, textureCoordinates).rgb;
    float specularExponent = texture(specular, textureCoordinates).a;

    // worldPosition = modelTransform * vec4(vertexPosition, 1.0f);
    // TODO: put in vertex shader?
    vec4 shadowCoord = shadowTransform * worldPosition;

    // Perspective divide.
    vec2 shadowIndex = shadowCoord.xy / shadowCoord.w;
    bool shadowed = false;
    if (shadowCoord.w > 0.0f && InRange(shadowIndex.x, 0.0f, 1.0f) && InRange(shadowIndex.y, 0.0f, 1.0f)) {
        float lightDepth = texture(shadow, shadowIndex).r; // Arbitrary, can be r, g, or b.
        float pixelDepth = shadowCoord.z;

        shadowed = pixelDepth > lightDepth + 0.001f;
    }

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

    vec3 color;

    if (shadowed) {
        color = ambientComponent;
    }
    else {
        color = (ambientComponent + diffuseComponent + specularComponent);
    }

    fragColor = vec4(color * lightBrightness, 1.0f);
}
