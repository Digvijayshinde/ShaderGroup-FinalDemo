#version 460 core

layout (location = 0) out vec4 color0_scene;
layout (location = 1) out vec4 color1_bright;
layout (location = 2) out vec4 color2_occlusion;

in vec2 waterTexcoord; 
in vec3 fromLightPositionVector;
in vec3 toCameraVector;
in vec2 textureCoords;
in vec4 clipspace;

uniform sampler2D sampler_textureReflection;
uniform sampler2D sampler_textureRefraction;
uniform sampler2D sampler_textureDUDVMap;
uniform sampler2D sampler_texturewaterNormalMap;
uniform sampler2D sampler_texturedepthMap;
uniform float moveFactor;
uniform vec3 lightColor;
uniform float uTime;
uniform float amplitude;

const float wavelength = 0.5f;
const float shineDmper = 35.0f;
const float reflectivity = 0.7f;

uniform bool u_lightEnabled = false;

float calculateDepth(float depth, float near, float far) {
    return 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
}

void main()
{
	vec2 ndc = (clipspace.xy/clipspace.w)/2.0f + 0.5f;
	vec2 reflexctionTexcoord = vec2(ndc.x,-ndc.y);
	vec2 refractionTexcoord = vec2(ndc.x,ndc.y);

	vec2 delta = waterTexcoord - vec2(0.565,0.668f);
	float distance = length(delta);
	
	// Depth Calculation
	float near = 0.1f;
	float far = 100000.0f;
	float depth = texture(sampler_texturedepthMap,refractionTexcoord).r;
	float floorDistance = 2.0*near*far/(far+near-(2.0*depth-1.0)*(far-near));
	depth = gl_FragCoord.z;
	float waterDistance = 2.0*near*far/(far+near-(2.0*depth-1.0)*(far-near));
	float waterDepth = floorDistance -waterDistance;


	vec2 distortedTexCoords = texture(sampler_textureDUDVMap, vec2(textureCoords.x + moveFactor, textureCoords.y)).rg*0.1;
	distortedTexCoords = textureCoords + vec2(distortedTexCoords.x, distortedTexCoords.y+moveFactor);
	vec2 totaldistortion = (texture(sampler_textureDUDVMap, distortedTexCoords).rg * 2.0 - 1.0) * wavelength;
    
	totaldistortion *= clamp(waterDepth/20.0f,0.0f,1.0f);
	totaldistortion *= pow(1-distance,96.0);


	refractionTexcoord += totaldistortion;
	refractionTexcoord = clamp(refractionTexcoord,0.001,0.999);

	reflexctionTexcoord += totaldistortion;
	reflexctionTexcoord.x = clamp(reflexctionTexcoord.x,0.001,0.999);
	reflexctionTexcoord.y = clamp(reflexctionTexcoord.y,-0.999,-0.001);

	vec4 texColorReflection = texture(sampler_textureReflection, reflexctionTexcoord);
	vec4 texColorRefraction = texture(sampler_textureRefraction, refractionTexcoord);

	// distortedTexCoords *= pow(1-distance,96.0);
	//vec4 normalMapColor = texture(sampler_textureDUDVMap,distortedTexCoords);
    //vec3 normal = vec3(normalMapColor.r * 2.0f -1.0f,normalMapColor.b * 3.0f,normalMapColor.g * 2.0f - 1.0f);

	vec4 normalMapColor = texture(sampler_textureDUDVMap, distortedTexCoords);
	vec3 normal = normalize(vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.g * 2.0 - 1.0, normalMapColor.b * 2.0 - 1.0));

	normal = normalize(normal);

	vec3 viewVector = normalize(toCameraVector);
	float refractiveFactor = pow(clamp(dot(viewVector, normal), 0.0, 1.0), 1.0);

	vec3 reflectedLight = reflect(-normalize(fromLightPositionVector), normal);

	float specularIntensity = 1.0;
	float specular = pow(max(dot(reflectedLight, viewVector), 0.0), shineDmper) * specularIntensity;

	//specular = pow(specular, shineDmper);
	vec3 specularHighlights = lightColor * specular * reflectivity * clamp(waterDepth/5.0f,0.0f,1.0f);

	vec3 baseWaterColor = vec3(0.0, 0.3, 0.3); // Parameterize for flexibility
	vec4 waterMixColor = mix(texColorReflection, texColorRefraction, refractiveFactor);
	waterMixColor.rgb = mix(waterMixColor.rgb, baseWaterColor, 0.2);
	waterMixColor.rgb += specularHighlights;
	waterMixColor.a = clamp(waterDepth / 2.0, 0.0, 1.0);
	//FragColor = waterMixColor;//waterMixColor;//texColorReflection;// vec4(0.0f,0.0f,1.0f,1.0f);
	

	// code for bloom
	vec4 color = waterMixColor;
    color0_scene = color;
	color1_bright = vec4(0.0, 0.0, 0.0, 1.0);
	color2_occlusion = vec4(0.0, 0.0, 0.0, 1.0);
}
































