
#version 450 core

#define MAX_NUM_LIGHTS 256
#define PI 3.1415

// Inputs from the vertex shader.
in vec2 textureCoordinates;

// Definition of a light in GLSL.
struct Light {
    bool isActive;
    vec3 position;
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
};

// Light data uniform block.
layout (std140, binding = 1) uniform LightData {
    Light lights[MAX_NUM_LIGHTS];
} lightData;

// Texture samplers for various geometry buffers.
uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D ambient;
uniform sampler2D diffuse;
uniform sampler2D specular;

// Camera information (in world space).
uniform vec3 cameraPosition;
uniform mat4 viewTransform;
uniform float cameraNearPlane;
uniform float cameraFarPlane;

// Shader outputs.
layout (location = 0) out vec4 fragColor;

void main(void) {
    vec4 viewPosition = vec4(texture(position, textureCoordinates).rgb, 1.0f);
    vec4 viewNormal = vec4(texture(normal, textureCoordinates).rgb, 0.0f);
    vec3 ambientCoefficient = texture(ambient, textureCoordinates).rgb;
    vec3 diffuseCoefficient = texture(diffuse, textureCoordinates).rgb;
    vec3 specularCoefficient = texture(specular, textureCoordinates).rgb;
    float specularExponent = texture(specular, textureCoordinates).a;

    // View space calculations.
    vec3 color = vec3(0.0f);

    vec4 N = normalize(viewNormal);
    vec4 viewVector = viewTransform * vec4(cameraPosition, 1.0f) - viewPosition;
    vec4 V = normalize(viewVector);

    for (int i = 0; i < MAX_NUM_LIGHTS; ++i) {
        if (!lightData.lights[i].isActive) {
            continue;
        }

        // Light direction
        vec4 L = normalize(viewTransform * vec4(lightData.lights[i].position, 1.0f) - viewPosition);

        // Add ambient component.
        color += ambientCoefficient * lightData.lights[i].ambientColor;

        // Diffuse.
        float diffuseMultiplier = max(dot(N, L), 0.0f);
        vec3 diffuseComponent = lightData.lights[i].diffuseColor * diffuseCoefficient * diffuseMultiplier;

        // Specular
        vec3 specularComponent = vec3(0.0f);
        if (diffuseMultiplier > 0.0f) {
            vec4 H = normalize(L + V);

            float specularMultiplier = max(dot(N, H), 0.0f);

            specularComponent = lightData.lights[i].specularColor * specularCoefficient * pow(specularMultiplier, specularExponent);
        }

        color += (diffuseComponent + specularComponent);
    }

    //    float S = clamp((cameraFarPlane - length(viewVector)) / (cameraFarPlane - cameraNearPlane), 0.0f, 1.0f);
    //    vec4 outColor = vec4((S * color) + ((1.0f - S) * lightData.fogColor), 1.0f);
    fragColor = vec4(color, 1.0f);
}
