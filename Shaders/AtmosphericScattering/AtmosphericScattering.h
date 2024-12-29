 #pragma once
#include "../../Utility/Headers/ShaderManager.h"
#include "../../Source/Common.h"

class AtmosphericScatteringShader {
private:
	ShaderManager groundShaderProgramObject;
	ShaderManager skyShaderProgramObject;
	

	vec3 cameraPos;
	vec3  cameraFront;
	vec3 cameraUp;

	
	vec3 wavelength;
	GLfloat cameraHeight;
	GLfloat fscale;
	GLfloat rayleigh_constant = 0.0025f;
	GLfloat scaleDepth = 0.25f;
	GLfloat innerRadius = 10000.0f;
	GLfloat outerRadius = 10500.0f;
	GLfloat eSun = 20.0f;
	GLfloat g = -0.99f;

	//For sphere
	std::vector<float> vertices_inner_sphere;
	std::vector<int> indices_inner_sphere;
	std::vector<float> vertices_outer_sphere;
	std::vector<int> indices_outer_sphere;

	GLuint vao_inner_sphere;
	GLuint vbo_inner_sphere_position;
	GLuint vbo_inner_sphere_indices;

	GLuint vao_outer_sphere;
	GLuint vbo_outer_sphere_position;
	GLuint vbo_outer_sphere_indices;

	void GenerateSphere(float radius, float sectorCount, float stackCount, std::vector<float>& vertices, std::vector<int>& indices);

public:
	int initializeAtmosphericScatteringProgram();
	void useGroundAtmosphericScatteringProgram();
	void useSkyAtmosphericScatteringProgram();

	void displayGroundAtmosphericScatteringShader(mat4,mat4,vec3);
	void displaySkyAtmosphericScatteringShader(mat4,mat4,vec3);

	void unUseGroundAtmosphericScatteringProgram();
	void unUseSkyAtmosphericScatteringProgram();

	void deleteShaderProgramObject();

	void increaseLightPosition(int pos, float value);
	void decreaseLightPosition(int pos, float value);
	void increaseAndDecraseSunEmision(int flag,float value);

	vec3 lightPos;
	GLfloat mie_constant = 0.0550f;

};
