
#include "../Headers/TestCommon.h"
#include "../../Shaders/CubeMap/CubeMap.h"
#include "vector"
//variable declaration

static CubeMapShader *cubeMapShader = new CubeMapShader();
static unsigned int cubemapTexture;


static std::vector<std::string> facesCubeMap
{
	"Media\\Textures\\Test\\CubeMap\\right.bmp",
	"Media\\Textures\\Test\\CubeMap\\left.bmp",
	"Media\\Textures\\Test\\CubeMap\\top.bmp",
	"Media\\Textures\\Test\\CubeMap\\bottom.bmp",
	"Media\\Textures\\Test\\CubeMap\\front.bmp",
	"Media\\Textures\\Test\\CubeMap\\back.bmp"
};


int Test::initializeTestCubeMap() {

	TextureManager textureManager;

	//code
	if (cubeMapShader->initializeCubeMapShaderProgram() != 0) {
		return -1;
	}

	cubemapTexture = textureManager.loadCubeMapTexture(facesCubeMap);

	return 0;
}

void Test::displayTestCubeMap() {

	cubeMapShader->useCubeMapShaderProgram();

	cubeMapShader->displayCubeMapShader(cubemapTexture);
	renderes->renderCube();
	cubeMapShader->unUseCubeMapShaderProgram();
	
}
void Test::updateTestCubeMap() {

	//code
}
void Test::uninitializeTestCubeMap() {

	cubeMapShader->deleteShaderProgramObject();
	if (cubeMapShader) {
		delete cubeMapShader;
		cubeMapShader = NULL;
	}

}

