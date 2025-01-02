#version 460 core

layout (location = 0) in vec4 a_position;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform bool u_fullscreenfade;


void main(void)
{
	if(u_fullscreenfade){
		gl_Position = a_position;
	}
	else{
		gl_Position=u_projectionMatrix *u_viewMatrix*u_modelMatrix* a_position;
	}
};
