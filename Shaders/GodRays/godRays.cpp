#include "godRays.h"

int GodRaysShader::initializeGodRaysShaderProgram()
{
	std::vector<std::string> shaderName;
	shaderName.push_back("Shaders\\godRays\\GodRays.vert");
	shaderName.push_back("Shaders\\godRays\\GodRays.frag");
	if (shaderProgramObject.loadShaderAndCompileShader(shaderName) != 0) {
		PrintLog("loading of GodRays shader failed\n");
		return -1;
	}
	return 0;
}

void GodRaysShader::useGodRaysShaderProgram()
{
	shaderProgramObject.useProgram();
}

void GodRaysShader::displayGodRaysShader(mat4 modelMatrix, mat4 viewMatrix, GLuint texturePass1, GLuint texturePass2)
{
	shaderProgramObject.setMat4("u_modelMatrix", modelMatrix);
	shaderProgramObject.setMat4("u_viewMatrix", viewMatrix);
	shaderProgramObject.setMat4("u_projectionMatrix", prespectiveProjectionMatrix);

	shaderProgramObject.setFloat("uExposure", exposure);
	shaderProgramObject.setFloat("uDecay", decay);
	shaderProgramObject.setFloat("uDensity", density);
	shaderProgramObject.setFloat("uWeight", weight);
	shaderProgramObject.setVec2("manipulationFactor", manipulationFactor);
	shaderProgramObject.setVec4("lightPosition", lightPosition);


	glActiveTexture(GL_TEXTURE0);
	shaderProgramObject.setInt("u_texturesampler1", 0);
	glBindTexture(GL_TEXTURE_2D, texturePass1);
	glActiveTexture(GL_TEXTURE1);
	shaderProgramObject.setInt("u_texturesampler2", 1);
	glBindTexture(GL_TEXTURE_2D, texturePass2);

}

void GodRaysShader::unUseGodRaysShaderProgram()
{
	shaderProgramObject.unUseProgram();
}

void GodRaysShader::deleteShaderProgramObject()
{
	shaderProgramObject.uninitializeShader();
}
void GodRaysShader::setFrameTime(float value) {
	
}