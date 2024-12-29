#pragma once
#include "../../Utility/Headers/ShaderManager.h"
#include "../../Source/Common.h"

class FireShader {
private:
	ShaderManager shaderProgramObject;

	float frameTime = 0.0f;
	vec3 scrollSpeeds = vec3(1.3f, 2.1f, 2.3f);
	vec3 scales = vec3(1.0f, 2.0f, 3.0f);
	vec2 distortion1 = vec2(0.1f, 0.2f);
	vec2 distortion2 = vec2(0.1f, 0.3f);
	vec2 distortion3 = vec2(0.1f, 0.1f);
	float distortionScale = 0.8f;
	float distortionBias = 0.5f;
public:
	int initializeFireShaderProgram();
	void useFireShaderProgram();
	void displayFireShader(mat4, mat4, GLuint, GLuint, GLuint);
	void unUseFireShaderProgram();
	void deleteShaderProgramObject();
	void setFrameTime(float value);
};
