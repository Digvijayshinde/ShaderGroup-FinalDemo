#pragma once
#include "../../Utility/Headers/ShaderManager.h"
#include "../../Source/Common.h"

class FadeInFadeOutShader {
private:
	ShaderManager shaderProgramObject;
public:
	int initializeFadeInFadeOutShaderProgram(void);
	void useFadeInFadeOutShaderProgram();
	void displayFadeInFadeOutShader(mat4 modelMatrix, mat4 viewMatrix, bool isFullScreenFade, vec3 color,float blendfactor);
	void unUseFadeInFadeOutShaderProgram();
	void deleteShaderProgramObject();
	void updateFadeInFadeOutShader(float);

	void SetMode(int flag);

	void FadeInFadeOut();

	float blendfactor = 0.0f;
	int mode = 0;
	vec3 color = vec3(0.0f,0.0f,0.0f);
	float fadeInFadeOutSpeed = 0.001f;

};
