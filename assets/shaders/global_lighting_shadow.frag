
#version 450 core

// Inputs from the vertex shader.
in vec2 textureCoordinates; // TODO: maybe use glFragCoord?

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
uniform float shadowBias;

// Shader outputs.
layout (location = 0) out vec4 fragColor;

bool InRange(float value, float low, float high) {
    return value >= low && value <= high;
}

bool IsShadowed(vec4 worldPosition, vec4 worldNormal) {
    bool isShadowed = false;

    // 'shadowCoordinate' is in range [-1.0f, 1.0f] (NDC space).
    vec4 shadowCoordinate = shadowTransform * worldPosition; // Where worldPosition = modelTransform * vec4(vertexPosition, 1.0f)

    // Since 'shadowMap' is a texture, 'shadowCoordinate' needs to be converted to texture coordinates [0.0f, 1.0f].
    shadowCoordinate = (shadowCoordinate + 1.0f) / 2.0f;

    // Check 1: don't project things behind the position of the light.
    if (shadowCoordinate.w > 0.0f) {
        // Since this value was not computed on the vertex shader, GLSL has not performed the perspective divide to transform
        // the shadow coordinate (range: [0.0f, w]) to NDC (range: [0.0f, 1.0f]). This needs to be done manually.
        vec3 shadowIndex = shadowCoordinate.xyz /= shadowCoordinate.w;
        float fragmentDepth = shadowIndex.z;

        // Checks 2 - 5: don't project things outside the view frustum of the light.
        if (InRange(shadowIndex.x, 0.0f, 1.0f) && InRange(shadowIndex.y, 0.0f, 1.0f)) {
            float shadowMapDepth = texture(shadowMap, shadowIndex.xy).r; // Reading from depth map, can be r, g, or b.

            // Adjust shadow bias offset based on the angle between the surface normal and light direction.
            // Steeper angles with a hardcoded shadow bias value may still show shadow acne.
            float shadowBias = max(0.005f * (1.0f - dot(worldNormal.xyz, lightDirection)), shadowBias);
            isShadowed = fragmentDepth > shadowMapDepth + shadowBias;
        }
    }

    return isShadowed;
}

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

    vec3 color;

    if (IsShadowed(worldPosition, N)) {
        color = ambientComponent;
    }
    else {
        color = (ambientComponent + diffuseComponent + specularComponent);
    }

    fragColor = vec4(color * lightBrightness, 1.0f);
}
