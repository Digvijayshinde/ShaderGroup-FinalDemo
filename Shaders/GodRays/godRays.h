#pragma once
#include "../../Utility/Headers/ShaderManager.h"
#include "../../Source/Common.h"

class GodRaysShader {
private:
	ShaderManager shaderProgramObject;
public:
	int initializeGodRaysShaderProgram();
	void useGodRaysShaderProgram();
	void displayGodRaysShader(mat4, mat4, GLuint, GLuint);
	void unUseGodRaysShaderProgram();
	void deleteShaderProgramObject();
	void setFrameTime(float value);

	float exposure = 0.0250f;
	float weight = 0.500;
	float decay = 0.990;
	float density = 0.2;
	vec2 manipulationFactor = vec2(0.07f, 0.09f);
	bool isColor = 0;
	vec4 lightPosition = vec4(1.500000, 0.400000, -0.200000, 1.0f);

};
