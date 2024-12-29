#include "Fire.h"

int FireShader::initializeFireShaderProgram()
{
	std::vector<std::string> shaderName;
	shaderName.push_back("Shaders\\Fire\\Fire.vert");
	shaderName.push_back("Shaders\\Fire\\Fire.frag");
	if (shaderProgramObject.loadShaderAndCompileShader(shaderName) != 0) {
		PrintLog("loading of Fire shader failed\n");
		return -1;
	}
	return 0;
}

void FireShader::useFireShaderProgram()
{
	shaderProgramObject.useProgram();
}

void FireShader::displayFireShader(mat4 modelMatrix, mat4 viewMatrix,GLuint alphaTexture,GLuint fireTexture,GLuint noiseTexture  )
{
	shaderProgramObject.setMat4("u_modelMatrix",modelMatrix);
	shaderProgramObject.setMat4("u_viewMatrix", viewMatrix);
	shaderProgramObject.setMat4("u_projectionMatrix", prespectiveProjectionMatrix);

	shaderProgramObject.setFloat("u_frameTime", frameTime);
	shaderProgramObject.setVec3("u_scrollSpeeds", scrollSpeeds);
	shaderProgramObject.setVec3("u_scales", scales);
	shaderProgramObject.setVec2("u_distortion1", distortion1);
	shaderProgramObject.setVec2("u_distortion2", distortion2);
	shaderProgramObject.setVec2("u_distortion3", distortion3);
	shaderProgramObject.setFloat("u_distortionScale", distortionScale);
	shaderProgramObject.setFloat("u_distortionBias", distortionBias);


	glActiveTexture(GL_TEXTURE0);
	shaderProgramObject.setInt("u_fireSampler", 0);
	glBindTexture(GL_TEXTURE_2D, fireTexture);
	glActiveTexture(GL_TEXTURE1);
	shaderProgramObject.setInt("u_noiseSampler", 1);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glActiveTexture(GL_TEXTURE2);
	shaderProgramObject.setInt("u_alphaSampler", 2);
	glBindTexture(GL_TEXTURE_2D, alphaTexture);

}

void FireShader::unUseFireShaderProgram()
{
	shaderProgramObject.unUseProgram();
}

void FireShader::deleteShaderProgramObject()
{
	shaderProgramObject.uninitializeShader();
}
void FireShader::setFrameTime(float value) {
	frameTime += value;
}