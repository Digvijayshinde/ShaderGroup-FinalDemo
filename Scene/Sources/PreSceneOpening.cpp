#include"../../Source/Globals.h"
#include "../../Scene/Headers/Scene.h"
#include "../../Shaders/AtmosphericScattering/AtmosphericScattering.h"
#include "../../Shaders/Terrain/Terrain.h"
#include "../../Shaders/ObjModelLoading/ObjModelLoading.h"
#include "../../Utility/Headers/ModelLoading/StaticModelLoader.h"
#include "../../Shaders/CommomTexture/Texture.h"
#include "../../Utility/Headers/AudioPlayer.h"
#include "../../../Utility/Headers/BezierCamera.h"
#include "../../Shaders/CubeMap/CubeMap.h"
#include "vector"
static TextureShader* textureShader = new TextureShader();

static AtmosphericScatteringShader* atmosphericScatteringShader = new AtmosphericScatteringShader();

static ObjModelLoadingShader* objmodelLoadingShader = new ObjModelLoadingShader();;

static int initCameraForPreSceneOpening = 0;
static int initSongForPreSceneOpening = 0;

static BezierCamera* bazierCameraForPreSceneOpening = new BezierCamera();

static TerrainShader* terrainShader = new TerrainShader();
static TextureManager* textures = new TextureManager();
static TextureManager* terrainTextures = new TextureManager();

static CubeMapShader* cubeMapShader = new CubeMapShader();
static unsigned int cubemapTexture;
static std::vector<std::string> facesCubeMap
{
	"Media\\Textures\\CubeMap\\right.jpg",
	"Media\\Textures\\CubeMap\\left.jpg",
	"Media\\Textures\\CubeMap\\up.jpg",
	"Media\\Textures\\CubeMap\\down.jpg",
	"Media\\Textures\\CubeMap\\front.jpg",
	"Media\\Textures\\CubeMap\\back.jpg"
};

extern StaticModel palace;

void Scene::WndProcForPreSceneOpening(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {
	//code
}

int Scene::initialisePreSceneOpening() {
	TextureManager textureManager;

	// Atmosphere Initialization
	if (atmosphericScatteringShader->initializeAtmosphericScatteringProgram() == -1)
		return -1;

	if (objmodelLoadingShader->initializeObjModelLoadingShaderProgram() != 0) {
		return -1;
	}
	initializeStaticModel(&palace, "Media/Models/CityWithCastle/city2.obj");
	textures->storeTextureFromFile("Media\\Textures\\Test", "Stone.bmp", ID_BITMAP_STONE);

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

	// Cubemap
	if (cubeMapShader->initializeCubeMapShaderProgram() != 0) {
		return -1;
	}
	cubemapTexture = textureManager.loadCubeMapTexture(facesCubeMap);

	//Bazier
	std::vector<std::vector<float>> points;
	points.push_back({ -793.819824,179.511719,360.568756 });
	points.push_back({ 19.139036,106.621033,202.988 });
	points.push_back({ 648.807068,129.611618,143.889 });
	points.push_back({ 615.762024,167.396851,-306.5449 });
	points.push_back({ 600.323608,176.346024,-662.1439 });
	//points.push_back({ 614.572144,178.287354,-649.010 });

	// texture model
	if (textureShader->initializeTextureShaderProgram() != 0) {
		return -1;
	}
	textures->storeTextureFromFile("Media\\Textures\\Test", "Stone.bmp", ID_BITMAP_STONE);

	std::vector<float> yaw;
	yaw.push_back(-41.400436
	);
	yaw.push_back(-88.600);
	yaw.push_back(-146.599);
	yaw.push_back(-179.5991);
	yaw.push_back(-214.9986);
	//yaw.push_back(143.202);


	std::vector<float> pitch;
	pitch.push_back(-3.6000);
	pitch.push_back(-6.800);
	pitch.push_back(-6.2000);
	pitch.push_back(-6.800);
	pitch.push_back(-9.80);
	//pitch.push_back(-8.599);

	bazierCameraForPreSceneOpening->initialize();
	bazierCameraForPreSceneOpening->setBezierPoints(points, yaw, pitch);
	bazierCameraForPreSceneOpening->time = 0;

	return 0;
}

void Scene::displayPreSceneOpening() {
	mat4 modelMatrixArray[1];

	if (initCameraForPreSceneOpening == 0) {
		initCameraForPreSceneOpening = 1;
		freeCamera->position = vec3(182.445862,  17.948416, 243.554276);
		freeCamera->front = vec3(0.999458, 0.027922, 0.017446);
		freeCamera->yaw = 1.000035;
		freeCamera->updateCameraVectors();
	}
	if (initSongForPreSceneOpening == 0) {
		if (AUDIO_ENABLE) {
			initSongForPreSceneOpening = 1;
			playSong(4);
		}
	}

	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	vec3 camPos;
	if (debugCamera)
	{
		viewMatrix = freeCamera->getViewMatrix();
		camPos = freeCamera->position;
	}
	else
	{
		viewMatrix = bazierCameraForPreSceneOpening->getViewMatrix();
		camPos = bazierCameraForPreSceneOpening->getCameraPosition();
	}

	//modelMatrix = genrateModelMatrix(vec3(261.800140, 0.0, 269.100159), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	//atmosphericScatteringShader->useGroundAtmosphericScatteringProgram();
	//atmosphericScatteringShader->displayGroundAtmosphericScatteringShader(modelMatrix, viewMatrix, camPos);
	//atmosphericScatteringShader->unUseGroundAtmosphericScatteringProgram();
	//atmosphericScatteringShader->useSkyAtmosphericScatteringProgram();
	//atmosphericScatteringShader->displaySkyAtmosphericScatteringShader(modelMatrix, viewMatrix, camPos);
	//atmosphericScatteringShader->unUseSkyAtmosphericScatteringProgram();

	modelDirectionLightStruct.directionLight_Direction = vec3(0.600739, 10000 + 163.49, 0.899);
	modelMatrixArray[0] = genrateModelMatrix(vec3(0.0, 0000 + 21, 0.0), vec3(0.0, 0.0, 0.0), vec3(5.900005, 5.900005, 5.90000));
	objmodelLoadingShader->displayObjModelLoadingShader(&palace, modelMatrixArray, viewMatrix,1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3(806.56+500.0, 0000.0f, 832.63+500.0), vec3(0.0, -180.30,0.0), vec3(1.0, 1.0, 1.0));
	terrainShader->useTerrainShaderProgram();
	terrainShader->lightPos = vec3(500.0, 0.0, 500.0);
	terrainShader->displayTerrainShader(terrainTextures, modelMatrix, viewMatrix, camPos, 68.5, 4.10);
	terrainShader->unUseTerrainShaderProgram();
	
	modelMatrix = genrateModelMatrix(vec3(100.800140,0000.0, 100.100159), vec3(0.0, 0.0, 0.0), vec3(2000.0, 2000.0, 2000.0));
	cubeMapShader->useCubeMapShaderProgram();
	cubeMapShader->displayCubeMapShader(cubemapTexture, modelMatrix, viewMatrix );
	renderes->renderCube();
	cubeMapShader->unUseCubeMapShaderProgram();

	modelMatrix = genrateModelMatrix(transformationVector.translationVector, vec3(0.0, 0.0, 0.0), vec3(10.0, 10.0, 10.0));
	textureShader->useTextureShaderProgram();
	textureShader->displayTextureShader(modelMatrix, viewMatrix, textures->getTexture("Stone"), 0);
	renderes->renderSphere();
	textureShader->unUseTextureShaderProgram();


	//glBindTexture(GL_TEXTURE_2D, 0);
}
void Scene::updatePreSceneOpening() {
	if (!debugCamera)
	{
		if (bazierCameraForPreSceneOpening->time < 1.0f) {
			bazierCameraForPreSceneOpening->time += 0.00025f;
			bazierCameraForPreSceneOpening->update();
		}
	}
}
void Scene::unitializePreSceneOpening() {

	atmosphericScatteringShader->deleteShaderProgramObject();
	if (atmosphericScatteringShader)
		delete atmosphericScatteringShader;

	objmodelLoadingShader->deleteShaderProgramObject();
	bazierCameraForPreSceneOpening->uninitialize();

	cubeMapShader->deleteShaderProgramObject();
	if (cubeMapShader) {
		delete cubeMapShader;
		cubeMapShader = NULL;
	}

	terrainShader->deleteShaderProgramObject();
	if (terrainShader) {
		delete terrainShader;
		terrainShader = NULL;
	}
}
