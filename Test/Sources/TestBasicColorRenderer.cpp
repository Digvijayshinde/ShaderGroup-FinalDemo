#include "../Headers/TestCommon.h"
#include "../../Shaders/CommonColor/Color.h"

//Creating object
static ColorShader* colorShader = new ColorShader();

int Test::initializeTestColor() {

	///code
	if (colorShader->initializeColorShaderProgram() != 0) {
		return 1;
	}

	return 0;
}
void Test::displayTestColor() {

	//code

	mat4 modelMatrix = mat4::identity();

	modelMatrix = genrateModelMatrix(transformationVector.translationVector, transformationVector.rotationVector,transformationVector.scaleVector);
	colorShader->useColorShaderProgram();
	colorShader->displayColorShader(modelMatrix, mat4::identity(), vec3(1.0f, 0.0f, 0.0f));
	renderes->renderQuad();
	colorShader->unUseColorShaderProgram();

}
void Test::updateTestColor() {

}
void Test::uninitializeTestColor() {

	//code
	colorShader->deleteShaderProgramObject();
}
