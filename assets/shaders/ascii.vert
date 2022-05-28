
#version 460 core

// layout (column_major) uniform;
// layout (row_major) buffer;

layout(binding = 3, row_major) uniform MatrixBlock {
    layout (column_major) mat4 projection;
    mat4 modelview;
} matrices;

layout (std430, binding = 1) buffer ssbo {
    mat4 d;
} sb1;

layout (location = 0) in vec3 vertexPosition;

void main() {
    gl_Position = vec4(vec3(matrices.projection * matrices.modelview * vec4(vertexPosition, 1.0f)), 1.0);
}

