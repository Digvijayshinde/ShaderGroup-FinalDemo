#version 300 es

layout(location = 0) in vec4 aPosition;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main() {
  gl_Position = lightSpaceMatrix * model * aPosition;
}
