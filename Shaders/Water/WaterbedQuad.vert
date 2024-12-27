#version 460 core 

in vec4 a_position;

uniform mat4 u_modelMatrix; 
uniform mat4 u_viewMatrix; 
uniform mat4 u_projectionMatrix; 

uniform vec3 a_color;
out vec3 a_color_out;
void main(void) 
{ 
    gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(a_position.x, 0.0f, a_position.y, 1.0); 
    a_color_out = a_color;
}
