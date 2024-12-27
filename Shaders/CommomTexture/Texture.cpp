#pragma once
#include "Texture.h"
int TextureShader :: initializeTextureShaderProgram() {

	std::vector<std::string> shaderName;
	shaderName.push_back("Shaders\\CommomTexture\\CommonTexture.vert");
	shaderName.push_back("Shaders\\CommomTexture\\CommonTexture.frag");
	return shaderProgramObject.loadShaderAndCompileShader(shaderName);

}
void TextureShader ::useTextureShaderProgram() {
	shaderProgramObject.useProgram();
}
void TextureShader ::displayTextureShader(mat4 modelMatrix ,mat4 viewMatrix,GLuint textureId,int isFullScreen) {


	shaderProgramObject.setMat4("u_modelMatrix", modelMatrix);
	shaderProgramObject.setMat4("u_viewMatrix", viewMatrix);
	shaderProgramObject.setMat4("u_projectionMatrix", prespectiveProjectionMatrix);
	shaderProgramObject.setInt("isFullScreen", isFullScreen);

	//activating texture 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	shaderProgramObject.setInt("u_textureSampler", 0);

}
void TextureShader::unUseTextureShaderProgram() {
	shaderProgramObject.unUseProgram();
}
void TextureShader::deleteShaderProgramObject(){
	shaderProgramObject.uninitializeShader();
}
