
#include "../Headers/TestCommon.h"
#include "../../Shaders/CommomTexture/Texture.h"

//variable declaration
static TextureManager *textures=new TextureManager();
static TextureShader *textureShader = new TextureShader();

void addMultipleOfSameModel() {

	if (ENABLE_MODEL_PLACING) {

		mat4 modelMatrix = mat4::identity();
		mat4 viewMatrix = mat4::identity();


		for (auto itr = transformationVectorArray.begin(); itr != transformationVectorArray.end(); itr++) {

			modelMatrix = genrateModelMatrix((*itr).translationVector, (*itr).rotationVector, (*itr).scaleVector);
			textureShader->useTextureShaderProgram();
			textureShader->displayTextureShader(modelMatrix, mat4::identity(), textures->getTexture("Stone"), 0);
			renderes->renderCube();
		}

		textureShader->unUseTextureShaderProgram();
		glBindTexture(GL_TEXTURE_2D, 0);

	}
}

int Test::initializeTestTexture() {

	//code
	if (textureShader->initializeTextureShaderProgram() != 0) {
		return -1;
	}

	textures->storeTextureFromFile("Media\\Textures\\Test", "wood.png");
	textures->storeTextureFromFile("Media\\Textures\\Test", "Smiley.bmp",ID_BITMAP_SMILEY);
	textures->storeTextureFromFile("Media\\Textures\\Test", "Stone.bmp", ID_BITMAP_STONE);
	textures->storeTextureFromFile("Media\\Textures\\Test", "alpha.dds");

	return 0;
}

void Test::displayTestTexture() {

	//code
	mat4 modelMatrix= genrateModelMatrix(transformationVector.translationVector, vec3(0.0,0.0,0.0), vec3(1.0, 1.0, 1.0));

	textureShader->useTextureShaderProgram();
	textureShader->displayTextureShader(modelMatrix,freeCamera->getViewMatrix(), textures->getTexture("Stone"), 0);
	renderes->renderSphere();
	textureShader->unUseTextureShaderProgram();
	glBindTexture(GL_TEXTURE_2D, 0);

	//***************************************
	// testing the model with runtime model matrix
	//***************************************
	//addMultipleOfSameModel();

}
void Test::updateTestTexture() {

	//code
}
void Test::uninitializeTestTexture() {

	//code
	textures->removeTextueCollection();

	if (textures) {
		delete(textures);
		textures = NULL;
	}
	textureShader->deleteShaderProgramObject();
}

