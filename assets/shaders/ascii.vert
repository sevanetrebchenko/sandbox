
#version 460 core

// layout (column_major) uniform;
// layout (row_major) buffer;

struct A {
    float c;
};

struct Test {
    float b;
    A a;
    A[10] array;
};

layout(std430, binding = 2) buffer MyBuffer {
    mat4 matrix;
    float lotsOfFloats[];
};

layout(binding = 3, row_major) uniform MatrixBlock {
    layout (column_major) mat4 projection;
    mat4 modelview;

    Test[5] tests;
} matrices;

//layout (std430, binding = 1) buffer ssbo {
//    mat4 d;
//} sb1;


layout (binding = 0) uniform sampler2D s;
layout (binding = 1) uniform texture2D t;
layout (binding = 2) uniform sampler   b;

layout (location = 0) in mat4 test;
layout (location = 5) in vec3 vertexPosition;
layout (location = 6) in float v;
layout (location = 8) in float array[1];

layout (location = 0) out BlockName {
    flat ivec3 someInts; // Flat interpolation.
    vec4 value;          // Default interpolation is smooth.
};

void main() {
    int a;
//    gl_Position = vec4(vec3(matrices.projection * matrices.modelview * vec4(vertexPosition, 1.0f)), 1.0);
    gl_Position = vec4(vertexPosition, 1.0f);
}

