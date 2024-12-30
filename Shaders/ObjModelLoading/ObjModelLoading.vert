#version 460 core

layout (location = 0) in vec4 a_position;
layout (location = 2) in vec3 a_normal;
layout (location = 3) in vec2 a_texcoord;

out vec2 a_out_texcoord;

out vec3 N;
out vec3 P;
out vec3 viewPos;
out vec3 Normal;
    

//uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_modelMatrix[200];

void main()
{
	gl_Position=u_projectionMatrix*u_viewMatrix*u_modelMatrix[gl_InstanceID]*a_position;
	
     N=mat3(u_modelMatrix[gl_InstanceID])*a_normal;
     P = vec3(u_modelMatrix[gl_InstanceID] * a_position);
     viewPos = u_viewMatrix[3].xyz;
    Normal = mat3(transpose(inverse(u_modelMatrix[gl_InstanceID]))) * a_normal;

	a_out_texcoord=a_texcoord;
};