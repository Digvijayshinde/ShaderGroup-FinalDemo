#pragma once
#include "FBufferEffect.h"

int FBufferEffectShader::initializFBufferEffectShaderProgram() {
	std::vector<std::string> shaderName;
	shaderName.push_back("Shaders\\FBufferEffect\\FBufferEffect.vert");
	shaderName.push_back("Shaders\\FBufferEffect\\FBufferEffect.frag");
	int retVal = shaderProgramObject.loadShaderAndCompileShader(shaderName);
	if (retVal != 0) {
		PrintLog("loading of FBufferEffect shader failed\n");
		return -1;
	}
	return 0;

}
void FBufferEffectShader::useFBufferEffectShaderProgram() {
	shaderProgramObject.useProgram();
}
void FBufferEffectShader::displayFBufferEffectShader(mat4 modelMatrix, mat4 viewMatrix, GLuint textureID, float intensity,float radius,float FBMTime,int effctType) {

	shaderProgramObject.setInt("u_effectType", effctType);


	if (effctType == 1) {
		//Vignite related code start
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		shaderProgramObject.setInt("u_textureSampler", 0);
		float resolution[] = { (float)gWinWidth, (float)gWinHeight };
		glUniform2f(glGetUniformLocation(shaderProgramObject.getShaderProgramObject(), "u_resolution"), gWinWidth, gWinHeight);
		shaderProgramObject.setFloat("u_intensity", intensity);
		shaderProgramObject.setFloat("u_radius", radius);
		//Vignitte relaetd code end
	}

	//FBM noise related code start
	else if (effctType == 2) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		shaderProgramObject.setInt("u_textureSampler", 0);
		shaderProgramObject.setFloat("u_time", FBMTime);
	}
	//FBM noise relaetd code end
}
void FBufferEffectShader::unUseFBufferEffectShaderProgram() {
	shaderProgramObject.unUseProgram();
}
void FBufferEffectShader::deleteShaderProgramObject() {
	shaderProgramObject.uninitializeShader();
}