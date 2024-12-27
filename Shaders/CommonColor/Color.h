#pragma once
#include "../../Utility/Headers/ShaderManager.h"
#include "../../Source/Common.h"

class ColorShader {
private:
	ShaderManager shaderProgramObject;
public:
	int initializeColorShaderProgram();
	void useColorShaderProgram();
	void displayColorShader(mat4 modelMatrix, mat4 viewMatrix,vec3 color);
	void unUseColorShaderProgram();
	void deleteShaderProgramObject();
};
