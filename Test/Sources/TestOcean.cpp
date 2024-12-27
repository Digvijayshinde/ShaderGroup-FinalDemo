
#include "../Headers/TestCommon.h"
#include "../../Shaders/Ocean/OceanShader.h"

static unsigned int cubemapTexture;

static std::vector<std::string> facesCubeMap
{
	"Media\\Textures\\Ocean\\right.jpg",
	"Media\\Textures\\Ocean\\left.jpg",
	"Media\\Textures\\Ocean\\top.jpg",
	"Media\\Textures\\Ocean\\bottom.jpg",
	"Media\\Textures\\Ocean\\front.jpg",
	"Media\\Textures\\Ocean\\back.jpg"
};


int Test::initializeTestOcean() {

	TextureManager textureManager;

	if (initializeOceanShader() != 0)
	{
		PrintLog("initializeOceanShader() failed\n");
		return (-1);
	}
	cubemapTexture = textureManager.loadCubeMapTexture(facesCubeMap);


	return 0;
}

void Test::displayTestOcean(float alpha, float elipseTime) {

	renderOcean(cubemapTexture, alpha,elipseTime);

}
void Test::updateTestOcean() {

	//code
	//alpha += 0.000001f;
	//elipseTime +=0.0001;
}
void Test::uninitializeTestOcean() {
}

//void Test::WndProcForTest(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {
//	
//	callbackOceanShader(hwnd, imsg, wParam, lParam);
//	}


