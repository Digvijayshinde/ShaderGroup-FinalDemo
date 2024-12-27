#version 460 core

layout (location = 0 ) in vec4 vPosition;
layout (location = 1 ) in vec4 vColor;
layout (location = 2 ) in vec4 vNormal;
layout (location = 3 ) in vec2 vTexCoord;

uniform mat4 u_modelMatrix;																		
uniform mat4 u_viewMatrix;																		
uniform mat4 u_projectionMatrix;
uniform vec3 cameraPosition;
uniform vec3 lightPosition;


out vec4 clipspace;
out vec2 textureCoords;
out vec3 toCameraVector;
out vec3 fromLightPositionVector;
out vec2 waterTexcoord;

const float tailing = 4.0f;

void main()
{
	vec4 worldPosition = u_modelMatrix * vec4(vPosition.x,0.0f,vPosition.y,1.0f);
	clipspace = u_projectionMatrix * u_viewMatrix * worldPosition;
	textureCoords = vec2(vPosition.x/2.0f + 0.5f,vPosition.y/2.0f + 0.5f) * tailing;
	toCameraVector = cameraPosition - worldPosition.xyz;
	fromLightPositionVector =  worldPosition.xyz - lightPosition;
	waterTexcoord = vec2(vPosition.x/2.0f + 0.5f,vPosition.y/2.0f + 0.5f);
	gl_Position = clipspace;
}