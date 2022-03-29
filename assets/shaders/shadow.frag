
#version 450 core

in vec4 position;
layout(location = 0) out vec4 fragColor;

void main() {
     fragColor = vec4(vec3(position.w), 1.0f);
}