#version 460 core

in vec3 a_texcoord_out;
uniform samplerCube cubemap;
out vec4 FragColor;
void main(void)
{
	FragColor=	texture(cubemap,a_texcoord_out);
}
