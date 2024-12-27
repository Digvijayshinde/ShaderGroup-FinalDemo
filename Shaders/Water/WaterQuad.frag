#version 460 core    

in vec4 clipSpaceCoords;
in vec2 a_texcoords_out;
in vec3 toCameraVector;
in vec3 lightDirection;

out vec4 FragColor; 

uniform sampler2D u_reflectionTextureSampler;
uniform sampler2D u_refractionTextureSampler;
uniform sampler2D u_waterDUDVMapTextureSampler;
uniform sampler2D u_waterNormalMapTextureSampler;

uniform float u_moveFactorOffset;

uniform vec3 u_lightColor;

const float waveStrength = 0.02;

const float shininess = 20.0;
const float reflectivity = 0.5;

void main(void)    
{    
    // Convert Clip Space Coordinates To ScreenSpace Coordinates
    vec2 screenSpaceCoords;
    screenSpaceCoords.x = clipSpaceCoords.x / clipSpaceCoords.w;
    screenSpaceCoords.y = clipSpaceCoords.y / clipSpaceCoords.w;

    // Convert Screen Space Coordinates To Normalized Device Coordinates
    vec2 ndcCoords;
    ndcCoords.x = screenSpaceCoords.x / 2.0 + 0.5;
    ndcCoords.y = screenSpaceCoords.y / 2.0 + 0.5;

    // Calculate Total Distortion
    // vec2 distortion1 = (texture(u_waterDUDVMapTextureSampler, vec2(a_texcoords_out.x + u_moveFactorOffset, a_texcoords_out.y)).rg * 2.0 - 1.0) * waveStrength;
    // vec2 distortion2 = (texture(u_waterDUDVMapTextureSampler, vec2(-a_texcoords_out.x + u_moveFactorOffset, a_texcoords_out.y + u_moveFactorOffset)).rg * 2.0 - 1.0) * waveStrength;

    // vec2 totalDistortions = distortion1 + distortion2;

    vec2 distortedTexCoords = texture(u_waterDUDVMapTextureSampler, vec2(a_texcoords_out.x + u_moveFactorOffset, a_texcoords_out.y)).rg * 0.1;
    distortedTexCoords = a_texcoords_out + vec2(distortedTexCoords.x, distortedTexCoords.y + u_moveFactorOffset);
    vec2 totalDistortions = (texture(u_waterDUDVMapTextureSampler, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;

    // -------------------------------------------------------------------------------
    // Calculate Reflection Texture Coordinates Using NDC
    vec2 reflectTexcoords = vec2(ndcCoords.x, -ndcCoords.y);

    // Add Distortion Texture Coordinated Between 0.001 & 0.999
    reflectTexcoords = reflectTexcoords + totalDistortions;

    // CLAMP
    reflectTexcoords.x = clamp(reflectTexcoords.x, 0.001, 0.999);
    reflectTexcoords.y = clamp(reflectTexcoords.y, -0.999, -0.001);

    // Final
    vec4 reflectColor = texture(u_reflectionTextureSampler, reflectTexcoords);


    // -------------------------------------------------------------------------------
    // Calculate Refraction Texture Coordinates Using NDC
    vec2 refractTexcoords = vec2(ndcCoords.x, ndcCoords.y);

    // Add Distortion Texture Coordinated Between 0.001 & 0.999
    refractTexcoords = refractTexcoords + totalDistortions;

    // CLAMP
    refractTexcoords.x = clamp(refractTexcoords.x, 0.001, 0.999);
    refractTexcoords.y = clamp(refractTexcoords.y, 0.001, 0.999);

    // Final
    vec4 refractColor = texture(u_refractionTextureSampler, refractTexcoords);

    // FRESNEL EFFECT
    vec3 viewVector = normalize(toCameraVector);
    vec3 waterSurfaceNormal = vec3(0.0, 1.0, 0.0);
    float refractiveFactor = dot(viewVector, waterSurfaceNormal);
    refractiveFactor = pow(refractiveFactor, 2.0);  // Higher Power Greater Reflectivity

    // Color
    vec4 color = mix(reflectColor, refractColor, refractiveFactor);

    // For Specular Highlights
    vec4 normalMapColor = texture(u_waterNormalMapTextureSampler, totalDistortions);
    vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0);
    normal = normalize(normal);

    vec3 reflectedLight = reflect(normalize(lightDirection), normal);
    float specular = max(dot(reflectedLight, viewVector), 0.0);
    specular = pow(specular, shininess);
    vec3 specularHighlights = u_lightColor * specular * reflectivity;

    FragColor = mix(color, vec4(0.0, 0.3, 0.5, 1.0), 0.2) + vec4(specularHighlights, 0.0);   
}

