#pragma once
#include "../../Utility/Headers/ShaderManager.h"
#include "../../Source/Common.h"

class CubeMapShader {
private:
	ShaderManager shaderProgramObject;
public:
	int initializeCubeMapShaderProgram();
	void useCubeMapShaderProgram();
	void displayCubeMapShader(GLuint,mat4,mat4);
	void unUseCubeMapShaderProgram();
	void deleteShaderProgramObject();
};
