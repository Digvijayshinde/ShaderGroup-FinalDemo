#version 460 core
in vec2 a_out_texcoord;

out vec4 FragColor;

uniform float u_alpha;
uniform sampler2D u_textureSampler;

void main(void)
{

	FragColor=texture(u_textureSampler,a_out_texcoord);	
	if (FragColor.a < 0.1)
			discard;

	float darknessFactor = 0.2; // Adjust this value to control how dark the color becomes
    FragColor.rgb *= darknessFactor;

	FragColor=vec4(FragColor.rgb,u_alpha);
}