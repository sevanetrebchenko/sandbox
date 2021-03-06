
#version 450 core

#define MAX_NUM_CHARACTERS 96
#define MAX_NUM_BITMAPS 32

uniform sampler2D inputTexture;

// Bitmapped ascii character in GLSL.
struct AsciiCharacter {
    uvec4 bitmap[MAX_NUM_BITMAPS / 4]; // Arrays in UBOs get aligned to size of vec4, prevent waste of space.
};

// Font sheet.
layout (std140, binding = 1) uniform ASCIIData {
    ivec2 fontScale;
    uvec3 charactersInUse;
    AsciiCharacter fontsheet[MAX_NUM_CHARACTERS];
} characterData;


// Shader outputs.
layout(location = 0) out vec4 fragColor;

float IndexIntoBitmap(int characterIndex, int bitPosition) {
    AsciiCharacter character = characterData.fontsheet[characterIndex];

    // Get index into uvec4 array.
    int broadIndex = bitPosition / 128; // uvec4 index
    uvec4 broadData = character.bitmap[broadIndex];

    bitPosition -= broadIndex * 128; // 128 bytes in a uvec4.

    int narrowIndex = bitPosition / 32; // uint index.
    uint narrowData = broadData[narrowIndex];

    int bit = bitPosition % 32;

    return clamp((narrowData & (1u << bit)), 0.0f, 1.0f);
}

float Bitmap(int index, ivec2 normalizedPixelCoordinate) {
    if (clamp(normalizedPixelCoordinate.x, 0, characterData.fontScale.x - 1) == normalizedPixelCoordinate.x) {
        if (clamp(normalizedPixelCoordinate.y, 0, characterData.fontScale.y - 1) == normalizedPixelCoordinate.y) {
            int bitPosition = normalizedPixelCoordinate.x + characterData.fontScale.x * normalizedPixelCoordinate.y;
            return IndexIntoBitmap(index, bitPosition);
        }
    }
    return 0.0f;
}

void main() {
    vec2 pixel = gl_FragCoord.xy;
    vec3 textureColor = texture(inputTexture, floor(pixel / ((characterData.fontScale - 1) * 2)) * ((characterData.fontScale - 1) * 2) / vec2(2560, 1440)).rgb;

    float grayscaleColor = 0.3f * textureColor.r + 0.59f * textureColor.g + 0.11f * textureColor.b;
    uint numCharacters = characterData.charactersInUse.x;

    float intensityStep = 1.0f / numCharacters;
    float gray = intensityStep;

    // Calculate index of character to use.
    int index = 0;

    for (int i = 0; i < numCharacters; ++i) {
        if (grayscaleColor < gray) {
            break;
        }
        ++index;
        gray += intensityStep;
    }

    ivec2 coordinate = ivec2(floor(mod(pixel - 1.0f, characterData.fontScale + 3)) - 1);
    vec3 bitmapResult = vec3(Bitmap(index, coordinate));

    fragColor = vec4(textureColor * bitmapResult, 1.0);
//    fragColor = vec4(textureColor, 1.0);
}
