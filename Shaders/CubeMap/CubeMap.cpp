#pragma once
#include "CubeMap.h"

int CubeMapShader::initializeCubeMapShaderProgram() {
	std::vector<std::string> shaderName;
	shaderName.push_back("Shaders\\CubeMap\\CubeMap.vert");
	shaderName.push_back("Shaders\\CubeMap\\CubeMap.frag");
	int retVal = shaderProgramObject.loadShaderAndCompileShader(shaderName);
	if (retVal != 0) {
		PrintLog("loading of CubeMap shader failed\n");
		return -1;
	}
	return 0;

}
void CubeMapShader::useCubeMapShaderProgram() {
	shaderProgramObject.useProgram();
	glDepthMask(GL_FALSE);  //glDepthMask — enable or disable writing into the depth buffer

}
void CubeMapShader::displayCubeMapShader( GLuint textureID, mat4 modelMatrix, mat4 viewMatrix) {


	//activating texture 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	shaderProgramObject.setInt("cubemap", 0);
	shaderProgramObject.setMat4("u_modelMatrix", modelMatrix);
	shaderProgramObject.setMat4("u_viewMatrix", viewMatrix);
	shaderProgramObject.setMat4("u_projectionMatrix", prespectiveProjectionMatrix);

}
void CubeMapShader::unUseCubeMapShaderProgram() {
	shaderProgramObject.unUseProgram();
	glDepthMask(GL_TRUE);  
}
void CubeMapShader::deleteShaderProgramObject() {
	shaderProgramObject.uninitializeShader();
}
