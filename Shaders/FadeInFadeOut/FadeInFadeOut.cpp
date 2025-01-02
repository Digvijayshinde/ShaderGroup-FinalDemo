#pragma once
#include "FadeInFadeOut.h"
int FadeInFadeOutShader::initializeFadeInFadeOutShaderProgram(void) {
	std::vector<std::string> shaderName;
	shaderName.push_back("Shaders\\FadeInFadeOut\\FadeInFadeOut.vert");
	shaderName.push_back("Shaders\\FadeInFadeOut\\FadeInFadeOut.frag");
	int retval= shaderProgramObject.loadShaderAndCompileShader(shaderName);
	if (retval != 0) {
		PrintLog("loading of FadeInFadeOut shader failed\n");
		return -1;
	}
	return 0;

}
void FadeInFadeOutShader::useFadeInFadeOutShaderProgram() {
	shaderProgramObject.useProgram();
}
void FadeInFadeOutShader::displayFadeInFadeOutShader(mat4 modelMatrix, mat4 viewMatrix, bool isFullScreenFade,vec3 color,float blendfactor) {

	shaderProgramObject.setMat4("u_modelMatrix", modelMatrix);
	shaderProgramObject.setMat4("u_viewMatrix", viewMatrix);
	shaderProgramObject.setMat4("u_projectionMatrix", prespectiveProjectionMatrix);
	shaderProgramObject.setInt("u_fullscreenfade", isFullScreenFade);
	shaderProgramObject.setVec3("u_color", color);
	shaderProgramObject.setFloat("u_blendfactor", blendfactor);


}
void FadeInFadeOutShader::unUseFadeInFadeOutShaderProgram() {
	shaderProgramObject.unUseProgram();
}
void FadeInFadeOutShader::deleteShaderProgramObject() {
	shaderProgramObject.uninitializeShader();
}

void FadeInFadeOutShader::updateFadeInFadeOutShader(float blendingFactor)
{

}

void FadeInFadeOutShader::SetMode(int flag)
{
	static int effectStarted = 0;
	if (flag == 0) {
		mode = 0;
		effectStarted = 0;
	}
	else if (flag == 1 && effectStarted ==0 ) {  //fade in-- color to dark
		mode = 1;
		effectStarted = 1;
		blendfactor = 0;
		color = vec3(0.0f,0.0f,0.0f);
	}
	else if(flag==2) {					  //dark to white
		mode = 2;
		effectStarted = 1;
		blendfactor = 1;
		color = vec3(0.0f, 0.0f, 0.0f);
	}
}

void FadeInFadeOutShader::FadeInFadeOut()
{
	//mat4 modelMatrix = vmath::translate(2.0f,0.0f,-3.0f);

	if (mode == 1) {  //fade in
		blendfactor += fadeInFadeOutSpeed;
		if (blendfactor > 1.0f) {
			SetMode(2);
		}
	}
	else if (mode == 2) {		//fade out
		blendfactor -= fadeInFadeOutSpeed;
		if (blendfactor < 0.0f) {
			SetMode(0);
		}
	}
	shaderProgramObject.useProgram();

	//shaderProgramObject.setMat4("u_modelMatrix", modelMatrix);
	//shaderProgramObject.setMat4("u_viewMatrix", freeCamera->getViewMatrix());
	//shaderProgramObject.setMat4("u_projectionMatrix", prespectiveProjectionMatrix);
	shaderProgramObject.setInt("u_fullscreenfade", 1);
	shaderProgramObject.setVec3("u_color", color);
	shaderProgramObject.setFloat("u_blendfactor", blendfactor);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	renderes->renderQuad();
	glDisable(GL_BLEND);
	shaderProgramObject.unUseProgram();
}
