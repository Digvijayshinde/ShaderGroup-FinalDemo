#pragma once
#include "../../Utility/Headers/ShaderManager.h"
#include "../../Source/Common.h"

class TextureShader {
private:
public:
	int initializeTextureShaderProgram();
	void useTextureShaderProgram();
	void displayTextureShader(mat4 modelMatrix, mat4 viewMatrix, GLuint textureId, int isFullScreen);
	void unUseTextureShaderProgram();
	void deleteShaderProgramObject();
	ShaderManager shaderProgramObject;

	bool enableGodRays = 0;
	bool isColor_godRays = 1;
};
