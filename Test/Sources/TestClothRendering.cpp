
#include "../Headers/TestCommon.h"
#include "../../Shaders/ClothRendering/ClothRendering.h"
//variable declaration

static ClothRenderingShader* clothRendering = new ClothRenderingShader();


int Test::initializeTestClothRendering() {


	//code
	if (clothRendering->initializeClothRenderingShaderProgram() != 0) {
		return -1;
	}
	return 0;
}

void Test::displayTestClothRendering() {

	clothRendering->useClothRenderingShaderProgram();

	mat4 modelmatrix = vmath::translate(-2.0f,1.0f,-4.0f);
	mat4 viewMatrix = mat4::identity();
	clothRendering->displayClothRenderingShader(modelmatrix,viewMatrix,vec4(1.0f,.0f,1.0f,1.0f));
	clothRendering->unUseClothRenderingShaderProgram();

}
void Test::updateTestClothRendering() {

	//code
	clothRendering->TimeStep();
}
void Test::uninitializeTestClothRendering() {

	clothRendering->deleteShaderProgramObject();
	if (clothRendering) {
		delete clothRendering;
		clothRendering = NULL;
	}
}

