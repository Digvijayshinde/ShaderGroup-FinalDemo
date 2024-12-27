#version 460 core

layout (location = 0) in vec4 a_position;
layout (location = 3) in vec2 a_texcoord;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform int isFullScreen;
uniform int isFlipRequired;


out vec2 a_out_texcoord;

void main(void)
{
	if(isFullScreen==1)
	{
		gl_Position= a_position;
	}
	else if(isFullScreen==0)
	{
		gl_Position=u_projectionMatrix* u_viewMatrix*u_modelMatrix* a_position;
	}
	if(isFlipRequired==1){
		vec2 flippedTexCoord = vec2(a_texcoord.x, 1.0 - a_texcoord.y);
		a_out_texcoord=flippedTexCoord;
	}
	else
	a_out_texcoord=a_texcoord;

};