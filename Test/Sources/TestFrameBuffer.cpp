
#include "../Headers/TestCommon.h"
#include"../../Shaders/FBufferEffect/FBufferEffect.h"
#include"../../Shaders/CommomTexture/Texture.h"
#include "../../Utility/Headers/FrameBuffer.h"

//variable declaration
static TextureManager* textures = new TextureManager();
static FBufferEffectShader *fBufferShader = new FBufferEffectShader();
static TextureShader* textureShader = new TextureShader();
FrameBuffer testFrameBuffer;;


//Vignitte
float intensity = 1.0f;
float radius = 0.5f;


//FBM
float FBMTime = 0.0f;

int Test::initializeTestFBORendering() {

	//code
	if (fBufferShader->initializFBufferEffectShaderProgram() != 0)
		return 1;

	//FrameBuffer creation 
	if (CreateFBO(FBO_WIDTH,FBO_HEIGHT, testFrameBuffer, 1) != true)
	{
		PrintLog("Failed to create FBO for godrays\n");
		return 1;
	}

	if (textureShader->initializeTextureShaderProgram() != 0)
		return 1;

	//texture laoding
	textures->storeTextureFromFile("Media\\Textures\\Test", "Stone.bmp", ID_BITMAP_STONE);

	return 0;
}

//Vignitte wndproc
//void Test::WndProcForTest(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {
//
//	//wndproc for test
//	switch (imsg) {
//		case WM_CHAR:
//			switch (wParam) {
//			case '1':
//				intensity = intensity + 0.5f;
//				break;
//			case '2':
//				intensity = intensity - 0.5f;
//				break;
//			case '3':
//				radius = radius + 0.2f;
//				break;
//			case '4':
//				radius = radius -0.2f;
//				break;
//			}
//		break;
//	}
//
//
//}

void Test::WndProcForTest(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {

	//wndproc for test
	switch (imsg) {
		case WM_CHAR:
			switch (wParam) {
			case '1':
				FBMTime = FBMTime + 0.5f;
				break;
			case '2':
				FBMTime = FBMTime - 0.5f;
			}
		break;
	}
}


void Test::displayTestFBORendering() {
	
	////rendering to frameBuffer start
	//glBindFramebuffer(GL_FRAMEBUFFER, testFrameBuffer.fbo);
	//{
	//	resizeFBO(FBO_WIDTH, FBO_HEIGHT);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	fBufferShader->useFBufferEffectShaderProgram();
	//	fBufferShader->displayFBufferEffectShader(mat4::identity(), mat4::identity(), textures->getTexture("Stone"), intensity, radius);
	//	renderes->renderQuad();
	//	fBufferShader->unUseFBufferEffectShaderProgram();
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//}
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	////rendering to frameBuffer end

	////Frame bufffer display start
	//glViewport(0, 0, gWinWidth, gWinHeight);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//textureShader->useTextureShaderProgram();
	//textureShader->displayTextureShader(mat4::identity(), mat4::identity(), testFrameBuffer.fboTexture[0], 1);
	//renderes->renderQuad();
	//textureShader->unUseTextureShaderProgram();
	//glBindTexture(GL_TEXTURE_2D, 0);
	////Frame bufffer display end

	//Fbm 
	fBufferShader->useFBufferEffectShaderProgram();
	fBufferShader->displayFBufferEffectShader(mat4::identity(), mat4::identity(), textures->getTexture("Stone"), intensity, radius,FBMTime,2);
	renderes->renderQuad();
	fBufferShader->unUseFBufferEffectShaderProgram();
	glBindTexture(GL_TEXTURE_2D, 0);

}
void Test::updateTestFBORendering() {
	
	//code
	FBMTime = FBMTime + 0.01f;
}
void Test::uninitializeTestFBORendering() {

	//code
	textures->removeTextueCollection();
	fBufferShader->deleteShaderProgramObject();
	textureShader->deleteShaderProgramObject();
	if (textures) {
		delete(textures);
		textures = NULL;
	}
}


