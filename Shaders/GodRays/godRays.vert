#version 460 core
layout (location = 0 ) in vec4 a_position; 
layout (location = 3 )in vec2 a_texcoord; 
out vec2 a_out_texcoord;
uniform mat4 u_modelMatrix; 
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
void main(void) 
{
//gl_Position=u_projectionMatrix*u_viewMatrix*u_modelMatrix*a_position; 
gl_Position=a_position; 
a_out_texcoord=a_texcoord;
}