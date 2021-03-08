
#version 450 core

#define MAX_NUM_LIGHTS 256
#define PI 3.1415

// Inputs from the vertex shader.
in vec4 viewPosition;
in vec4 viewNormal;

// Definition of a light in GLSL.
struct Light {
    bool isActive;
    vec3 position;
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
};

// Light data uniform block.
layout (std140, binding = 0) uniform LightData {
    Light lights[MAX_NUM_LIGHTS];
} lightData;

// Object material lighting coefficients.
uniform vec3 ambientCoefficient;
uniform vec3 diffuseCoefficient;
uniform vec3 specularCoefficient;
uniform float specularExponent;

// Camera information (in world space).
uniform vec3 cameraPosition;
uniform mat4 viewTransform;
uniform float cameraNearPlane;
uniform float cameraFarPlane;

// Shader outputs.
layout(location = 0) out vec4 fragColor;

void main(void) {
    vec3 color = vec3(0.0f);

    vec4 N = normalize(viewNormal);
    vec4 viewVector = (viewTransform * vec4(cameraPosition, 1.0f)) - viewPosition;
    vec4 V = normalize(viewVector);

    // Global ambient
//    color += lightData.ambientColor * ambientCoefficient;

    for (int i = 0; i < 16; ++i) {
        if (!lightData.lights[i].isActive) {
            continue;
        }

        vec4 L = normalize((viewTransform * vec4(lightData.lights[i].position, 1.0f)) - viewPosition);

        // Add ambient component.
        color += ambientCoefficient * lightData.lights[i].ambientColor;

        // Diffuse.
        float diffuseMultiplier = max(dot(N, L), 0.0f);
        vec3 diffuseComponent = lightData.lights[i].diffuseColor * diffuseCoefficient * diffuseMultiplier;

        // Specular
        vec3 specularComponent = vec3(0.0f);
        if (diffuseMultiplier > 0.0f) {
            vec4 R = normalize(2 * diffuseMultiplier * N - L); // 2 N.L * N - L

            float specularMultiplier = max(dot(R, V), 0.0f);

            if (specularMultiplier > 0.0f) {
                specularComponent = lightData.lights[i].specularColor * specularCoefficient * pow(max(dot(R, V), 0.0f), specularExponent);
            }
        }

        color += (diffuseComponent + specularComponent);
    }

//    float S = clamp((cameraFarPlane - length(viewVector)) / (cameraFarPlane - cameraNearPlane), 0.0f, 1.0f);
//    vec4 outColor = vec4((S * color) + ((1.0f - S) * lightData.fogColor), 1.0f);
    fragColor = vec4(color, 1.0f);
}
