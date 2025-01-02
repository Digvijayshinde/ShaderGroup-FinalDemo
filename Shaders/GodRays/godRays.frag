#version 460 core

out vec4 fragColor;
in vec2 a_out_texcoord;
uniform sampler2D u_texturesampler1;
uniform sampler2D u_texturesampler2;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

// Exposure: Adjusts the brightness of the god rays.
// Decay: Determines how quickly light intensity decreases with distance.
// Density: Influences the number of samples taken along the ray path.
// Weight: Modifies the contribution of each sample to the final effect.

// don't send viewMatrix in 3rd pass only, we will get the effect we want

uniform float uExposure;
uniform float uDecay;
uniform float uDensity;
uniform float uWeight;

uniform vec4 lightPosition;
uniform vec2 manipulationFactor;

int num_samples = 100;

void main(void)
{
  vec4 lightPosition3D = u_projectionMatrix * u_viewMatrix * u_modelMatrix * lightPosition; 
    lightPosition3D /= lightPosition3D.w; 
    lightPosition3D += vec4(1.0); 
    lightPosition3D *= 0.5; 

    vec2 lightPosition2D = lightPosition3D.xy; 

    vec2 deltaTexCoord = a_out_texcoord - ( lightPosition2D + vec2(-0.07 + manipulationFactor.x, 0.09 + manipulationFactor.y));
    vec2 texCoord = a_out_texcoord; 
    deltaTexCoord *= 1.0 /  float(num_samples) * uDensity; 
    float illuminationDecay = 1.0;

    for(int i=0; i < num_samples ; i++)
    { 
        texCoord -= deltaTexCoord;
        vec4 sampled = texture(u_texturesampler1, texCoord); 
        sampled *= illuminationDecay * uWeight;
        fragColor += sampled;
        
        illuminationDecay *= uDecay;
    } 

    fragColor *= uExposure;
    fragColor = vec4(fragColor.rgb, 1.0) * vec4(1.0, 0.95, 0.76, 1.0);

    fragColor += texture(u_texturesampler2, a_out_texcoord);

    //fragColor=texture(u_texturesampler2, a_out_texcoord);
}