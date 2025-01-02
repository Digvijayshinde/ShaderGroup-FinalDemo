#version 460 core
in vec2 a_out_texcoord;

out vec4 FragColor;

uniform float u_alpha;
uniform sampler2D u_textureSampler;
uniform int isColor_godRays;
uniform bool enableGodRays;

void main(void)
{

	FragColor=texture(u_textureSampler,a_out_texcoord);	
	if (FragColor.a < 0.1)
			discard;

	float darknessFactor = 0.2; // Adjust this value to control how dark the color becomes
    FragColor.rgb *= darknessFactor;

	  if(enableGodRays)
     {
            if(isColor_godRays==1)
            {
                FragColor = vec4(1.0);
            }
            else
            {
                FragColor = vec4(0.0);
            }
      }


	//FragColor=vec4(FragColor.rgb,u_alpha);
}