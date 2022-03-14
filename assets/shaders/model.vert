
#type adfasdfsaf

          #               version      330    core      ddd d dd



layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;

uniform mat4 cameraTransform;
uniform mat4 modelTransform;

void main() {
    gl_Position = cameraTransform * modelTransform * vec4(vertexPosition, 1.0f);
}