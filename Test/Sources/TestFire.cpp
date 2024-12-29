#include "../Headers/TestCommon.h"
#include"../../Shaders/Fire/Fire.h"
#include "../../Utility/Headers/TextureManager.h"

static FireShader* fireShader = new FireShader();
static TextureManager *texture = new TextureManager();

int  Test::initializeTestFireRendering() {

	if (fireShader->initializeFireShaderProgram() == -1)
		return -1;

	texture->storeTextureFromFile("Media\\Textures\\Fire", "alpha.dds");
	texture->storeTextureFromFile("Media\\Textures\\Fire", "fire.dds");
	texture->storeTextureFromFile("Media\\Textures\\Fire", "noise.dds");

	return 0;
}
void  Test::displayTestFireRendering() {
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();

	modelMatrix = vmath::translate(0.0f,0.0f,-4.0f);

	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, texture->getTexture("alpha"), texture->getTexture("fire"), texture->getTexture("noise"));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	renderes->renderQuard();
	glDisable(GL_BLEND);

	fireShader->unUseFireShaderProgram();

}
void  Test::updateTestFireRendering() {

	fireShader->setFrameTime(0.001);
}
void  Test::uninitializeTestFireRendering() {
	if (fireShader) {
		delete fireShader;
	}
	texture->removeTextueCollection();
	if (texture) {
		delete texture;
	}
}
