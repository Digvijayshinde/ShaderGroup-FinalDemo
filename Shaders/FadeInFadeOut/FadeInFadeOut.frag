#version 460 core
uniform float u_blendfactor;

uniform vec3 u_color;
out vec4 FragColor;

void main(void)
{
	FragColor = vec4(u_color,u_blendfactor);
}
