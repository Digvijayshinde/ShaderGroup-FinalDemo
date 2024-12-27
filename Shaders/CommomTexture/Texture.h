#pragma once
#include "../../Utility/Headers/ShaderManager.h"
#include "../../Source/Common.h"

class TextureShader {
private:
	ShaderManager shaderProgramObject;
public:
	int initializeTextureShaderProgram();
	void useTextureShaderProgram();
	void displayTextureShader(mat4 modelMatrix, mat4 viewMatrix, GLuint textureId, int isFullScreen);
	void unUseTextureShaderProgram();
	void deleteShaderProgramObject();
};
