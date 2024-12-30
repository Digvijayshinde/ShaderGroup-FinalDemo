#version 300 es 

layout(location = 0) in vec4 aPos;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = aPos;
}
