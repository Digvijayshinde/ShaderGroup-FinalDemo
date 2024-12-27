#pragma once
#include "../../Utility/Headers/ShaderManager.h"
#include "../../Source/Common.h"

class FBufferEffectShader {
private:
	ShaderManager shaderProgramObject;
public:
	int initializFBufferEffectShaderProgram();
	void useFBufferEffectShaderProgram();
	void displayFBufferEffectShader(mat4, mat4, GLuint textureID = 0, float intensity = 0, float radius = 0, float FBMTime = 0, int effctType = 1);
	void unUseFBufferEffectShaderProgram();
	void deleteShaderProgramObject();
};
