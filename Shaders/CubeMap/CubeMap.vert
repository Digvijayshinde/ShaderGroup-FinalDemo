#version 460 core

layout (location = 0) in vec4 a_position;

uniform mat4  u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

out vec3 a_texcoord_out;
void main(void)
{
	gl_Position=u_projectionMatrix*u_viewMatrix*u_modelMatrix*vec4(a_position.xyz,1.0f);
	a_texcoord_out =a_position.xyz;
};
