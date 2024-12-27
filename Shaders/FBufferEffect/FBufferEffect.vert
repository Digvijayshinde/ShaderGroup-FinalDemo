#version 460 core

layout (location = 0) in vec4 a_position;
layout (location = 1) in vec4 a_color;
layout (location = 3) in vec2 a_texcoord;


out vec4 a_color_out;
out vec2 a_texcoord_out;

void main(void)
{
	gl_Position=a_position;
	a_color_out = a_color;
	a_texcoord_out = a_texcoord;
}