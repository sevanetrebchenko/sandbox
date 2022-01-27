
#version 450 core

// Inputs from the vertex shader.
in vec2 textureCoordinates; // TODO: maybe use glFragCoord?

// Texture samplers for various geometry buffers.
uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D ambient;
uniform sampler2D diffuse;
uniform sampler2D specular;

// Camera information (in world space).
uniform vec3 cameraPosition;

// Global light information.
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform float lightBrightness;

// Shader outputs.
layout (location = 0) out vec4 fragColor;

void main(void) {
    vec4 worldPosition = vec4(texture(position, textureCoordinates).rgb, 1.0f);
    vec4 worldNormal = vec4(texture(normal, textureCoordinates).rgb, 0.0f);
    vec3 ambientCoefficient = texture(ambient, textureCoordinates).rgb;
    vec3 diffuseCoefficient = texture(diffuse, textureCoordinates).rgb;
    vec3 specularCoefficient = texture(specular, textureCoordinates).rgb;
    float specularExponent = texture(specular, textureCoordinates).a;

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

    vec3 color = (ambientComponent + diffuseComponent + specularComponent) * lightBrightness;
    fragColor = vec4(color, 1.0f);
}
