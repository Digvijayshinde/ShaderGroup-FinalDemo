
#include "../Headers/TestCommon.h"
#include "../../Shaders/Water/Water.h"
#include "../../Shaders/WaterNew/Water.h"
#include "../../Source/Common.h"
#include "../../Shaders/CommomTexture/Texture.h"

//variable declaration
static WaterShader* waterShader = new WaterShader();
static TextureManager *textureManager = new TextureManager();
static TextureShader* textureShader = new TextureShader();

extern Test test;

static WaterFrameBuffers* fbos;
static float moveFactor = 0.0f;
static bool isYFlipped = false;

void aboveWater(mat4 viewMatrix) {

	mat4 translationMatrix = vmath::translate(0.0f,0.75f,0.0f);
	mat4 scaleMatrix = vmath::scale(0.1f, 0.1f, 0.1f);
	mat4 modelMatrix = translationMatrix * scaleMatrix;
	textureShader->useTextureShaderProgram();
	textureShader->displayTextureShader(modelMatrix,viewMatrix, textureManager->getTexture("waterDUDV"), 0);
	renderes->renderPyramid();
	textureShader->unUseTextureShaderProgram();
}

int Test::initializeTestWaterRendering() {

	//code
	

	return 0;
}





void Test::displayTestWaterRendering() {

	//mat4 modelMatrix = genrateModelMatrix(vec3(transformationVector.translationVector[0], transformationVector.translationVector[1] + 1000.0f, transformationVector.translationVector[2]), transformationVector.rotationVector, transformationVector.scaleVector);
	//vec3 lightColor = { 1.0f, 1.0f, 1.0f };       // White Light
	//vec3 lightPosition = { 0.0f, 2.0f, -1.0f };   // White Light
	//moveFactor = moveFactor + WATER_WAVE_SPEED;

	//vec3 cameraPosition = vec3(0.0f, 1.0f, 2.0f);
	//vec3 cameraCentre = vec3(0.0f, 0.0f, 0.0f);
	//vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

	//waterShader->displayWaterShader(modelMatrix, freeCamera->position, freeCamera->front, freeCamera->up, lightColor, lightPosition, moveFactor, textureManager->getTexture("waterDUDV"), textureManager->getTexture("waterNormalMap"), aboveWater);

	//fbos->draw_water(textureManager->getTexture("waterDUDVmap"), textureManager->getTexture("waterNormalMap"));

}
void Test::updateTestWaterRendering() {

	//code
}
void Test::uninitializeTestWaterRendering() {

	waterShader->deleteShaderProgramObject();
	if (waterShader) {
		delete waterShader;
		waterShader = NULL;
	}
}


