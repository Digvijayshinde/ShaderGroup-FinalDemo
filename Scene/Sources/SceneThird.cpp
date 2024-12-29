#include"../../Source/Globals.h"
#include "../../Scene/Headers/Scene.h"
#include "../../Shaders/AtmosphericScattering/AtmosphericScattering.h"
#include "../../Shaders/Terrain/Terrain.h"
#include "../../Shaders/ObjModelLoading/ObjModelLoading.h"
#include "../../Utility/Headers/ModelLoading/StaticModelLoader.h"
#include "../../Shaders/CommomTexture/Texture.h"
#include "../../Shaders/WaterNew/Water.h"


static AtmosphericScatteringShader* atmosphericScatteringShader = new AtmosphericScatteringShader();
static TerrainShader* terrainShader = new TerrainShader();
static TextureManager* terrainTextures = new TextureManager();
static StaticModel hospitalRoom;
static ObjModelLoadingShader* objmodelLoadingShader = new ObjModelLoadingShader();
extern TextureShader* textureShader = new TextureShader();
extern TextureManager* textures = new TextureManager();
static TextureManager* textureManager = new TextureManager();

static WaterFrameBuffers* fbos;
static float moveFactor = 0.0f;
static bool isYFlipped = false;

static int initCameraForSceneThird=0;
void Scene::WndProcForSceneThird(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {
	//code
}

int Scene::initialiseSceneThird() {
	// Atmosphere Initialization
	if (atmosphericScatteringShader->initializeAtmosphericScatteringProgram() == -1)
		return -1;

	// Terrain initialization
	if (terrainShader->initializeTerrainShaderProgram("Media\\Textures\\Terrain\\height.png") != 0) {
		return -1;
	}
	else
	{
		fprintf(gpFile, "initialize initializeTerrainShaderProgram\n");
	}
	terrainTextures->storeTextureFromFile("Media\\Textures\\Terrain", "splat.png");
	terrainTextures->storeTextureFromFile("Media\\Textures\\Terrain", "grass5.png");
	terrainTextures->storeTextureFromFile("Media\\Textures\\Terrain", "grass_normal.png");
	terrainTextures->storeTextureFromFile("Media\\Textures\\Terrain", "rock.png");
	terrainTextures->storeTextureFromFile("Media\\Textures\\Terrain", "rock_normal.png");
	terrainTextures->storeTextureFromFile("Media\\Textures\\Terrain", "NormalMap.png");

	if (textureShader->initializeTextureShaderProgram() != 0) {
		return -1;
	}
	textures->storeTextureFromFile("Media\\Textures\\Test", "wood.png");
	textures->storeTextureFromFile("Media\\Textures\\Test", "Smiley.bmp", ID_BITMAP_SMILEY);
	textures->storeTextureFromFile("Media\\Textures\\Test", "Stone.bmp", ID_BITMAP_STONE);
	textures->storeTextureFromFile("Media\\Textures\\Test", "alpha.dds");

	//Water initialization start
	fbos = new WaterFrameBuffers();
	fbos->initialization_water();
	textureManager->storeTextureFromFile("Media\\Textures\\Water", "waterDUDVmap.png");
	textureManager->storeTextureFromFile("Media\\Textures\\Water", "waterNormalMap.png");

	textureManager->storeTextureFromFile("Media\\Textures\\Water", "fftDudv.png");
	textureManager->storeTextureFromFile("Media\\Textures\\Water", "fftNormal.png");
	//Water initialization end

	if (textureShader->initializeTextureShaderProgram() != 0) {
		return -1;
	}

	return 0;
}

void Scene::displaySceneThird() {

	if (initCameraForSceneThird == 0) {
		initCameraForSceneThird = 1;
		freeCamera->position = vec3(225.241943, 10.120395, 265.759705);
		freeCamera->front = vec3(225.241943, 10.120395, 265.759705);

		freeCamera->yaw = -124.4;
		freeCamera->updateCameraVectors();
	}
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();

	modelMatrix = genrateModelMatrix(transformationVector.translationVector, transformationVector.rotationVector, vec3(1.0, 1.0, 1.0));
	terrainShader->useTerrainShaderProgram();
	//terrainShader->displayTerrainShader(terrainTextures, modelMatrix);
	terrainShader->unUseTerrainShaderProgram();

	modelMatrix = genrateModelMatrix(vec3(261.800140, -300.0, 269.100159), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	//atmosphericScatteringShader->useGroundAtmosphericScatteringProgram();
	//atmosphericScatteringShader->displayGroundAtmosphericScatteringShader(modelMatrix);
	//atmosphericScatteringShader->unUseGroundAtmosphericScatteringProgram();
	//atmosphericScatteringShader->useSkyAtmosphericScatteringProgram();
	//atmosphericScatteringShader->displaySkyAtmosphericScatteringShader(modelMatrix);
	//atmosphericScatteringShader->unUseSkyAtmosphericScatteringProgram();


	//captureWaterReflectionAndRefractionForThirdScene();
	modelMatrix = genrateModelMatrix(vec3(313.600616, 8.300001, 232.900238), vec3(0.0, 0.0, 0.0), vec3(600.0, 600.0, 600.0));
	//fbos->draw_water(textureManager->getTexture("fftDudv"), textureManager->getTexture("fftNormal"),modelMatrix,viewMatrix,camPos);

	modelMatrix = genrateModelMatrix(vec3(24.266718,10055.152344,356.773499), vec3(0.0, 0.0, 0.0), vec3(10.0, 10.0, 10.0));
	textureShader->useTextureShaderProgram();
	textureShader->displayTextureShader(modelMatrix, freeCamera->getViewMatrix(), textures->getTexture("Stone"), 0);
	renderes->renderSphere();
	textureShader->unUseTextureShaderProgram();

	glBindTexture(GL_TEXTURE_2D, 0);
}
void Scene::updateSceneThird() {

}
void Scene::unitializeSceneThird() {

	atmosphericScatteringShader->deleteShaderProgramObject();
	if (atmosphericScatteringShader)
		delete atmosphericScatteringShader;

	terrainShader->deleteShaderProgramObject();
	if (terrainShader) {
		delete terrainShader;
		terrainShader = NULL;
	}
}