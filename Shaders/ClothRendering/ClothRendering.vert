#version 460 core
layout (location = 0) in vec4 vPosition;
layout (location = 2) in vec3 vNormal;
uniform mat4 u_mvMatrix;
uniform mat4 u_projectionMatrix;
out vec3 out_vPosition;
out vec3 out_vNormal;
out vec3 out_viewVector;
void main(void)
{
	float scale = 1.0;
	vec4 eye_coordinates = u_mvMatrix * vPosition;
	gl_Position = u_projectionMatrix * u_mvMatrix * vPosition;
	out_vPosition = vec3(vPosition);
	out_viewVector = -eye_coordinates.xyz;
	out_vNormal = normalize(mat3(u_mvMatrix) * vNormal);
};


