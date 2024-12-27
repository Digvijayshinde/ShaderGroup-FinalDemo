#pragma once
#include "Color.h"
int ColorShader::initializeColorShaderProgram() {
	std::vector<std::string> shaderName;
	shaderName.push_back("Shaders\\CommonColor\\CommonColor.vert");
	shaderName.push_back("Shaders\\CommonColor\\CommonColor.frag");
	int retval= shaderProgramObject.loadShaderAndCompileShader(shaderName);
	if (retval != 0) {
		PrintLog("loading of color shader failed\n");
		return -1;
	}
	return 0;

}
void ColorShader::useColorShaderProgram() {
	shaderProgramObject.useProgram();
}
void ColorShader::displayColorShader(mat4 modelMatrix, mat4 viewMatrix,vec3 color) {

	shaderProgramObject.setMat4("u_modelMatrix", modelMatrix);
	shaderProgramObject.setMat4("u_viewMatrix", viewMatrix);
	shaderProgramObject.setMat4("u_projectionMatrix", prespectiveProjectionMatrix);
	shaderProgramObject.setVec3("color", color);

}
void ColorShader::unUseColorShaderProgram() {
	shaderProgramObject.unUseProgram();
}
void ColorShader::deleteShaderProgramObject() {
	shaderProgramObject.uninitializeShader();
}
