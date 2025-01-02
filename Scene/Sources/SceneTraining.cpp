#include"../../Source/Globals.h"
#include "../../Scene/Headers/Scene.h"
#include "../../Shaders/ObjModelLoading/ObjModelLoading.h"
#include "../../Utility/Headers/ModelLoading/StaticModelLoader.h"
#include "../../Shaders/CommomTexture/Texture.h"
#include "../../Utility/Headers/AudioPlayer.h"
#include "../../../Utility/Headers/BezierCamera.h"
#include"../../Shaders/Fire/Fire.h"
#include "../../Shaders/Terrain/Terrain.h"
#include "../../Shaders/CubeMap/CubeMap.h"

static TextureShader* textureShader = new TextureShader();
static FireShader* fireShader = new FireShader();

static ObjModelLoadingShader* objmodelLoadingShader = new ObjModelLoadingShader();;
static int initCameraForSceneTraining = 0;
static int initSongForSceneTraining = 0;

static BezierCamera* bazierCameraForSceneTraining = new BezierCamera();
static BezierCamera* bazierCameraForSceneTrainingTwo = new BezierCamera();

static TextureManager* textures = new TextureManager();
static TextureManager* terrainTextures = new TextureManager();
static TerrainShader* terrainShader = new TerrainShader();

static StaticModel seat;
extern StaticModel tree;
static StaticModel chandragupta;
extern StaticModel chanakyaStanding;

static int camNum = 0;

static CubeMapShader* cubeMapShader = new CubeMapShader();
static unsigned int cubemapTexture;

static std::vector<std::string> facesCubeMap
{
	"Media\\Textures\\CubemapMeet\\right.jpg",
	"Media\\Textures\\CubemapMeet\\left.jpg",
	"Media\\Textures\\CubemapMeet\\up.jpg",
	"Media\\Textures\\CubemapMeet\\down.jpg",
	"Media\\Textures\\CubemapMeet\\front.jpg",
	"Media\\Textures\\CubemapMeet\\back.jpg"
};


void Scene::WndProcForSceneTraining(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {
	//code
}

int Scene::initialiseSceneTraining() {

	// Atmosphere Initialization
	if (objmodelLoadingShader->initializeObjModelLoadingShaderProgram() != 0) {
		return -1;
	}
	initializeStaticModel(&seat, "Media/Models/chanakya_chandra_Meet/seat.obj");
	initializeStaticModel(&chandragupta, "Media/Models/chanakya_chandra_Meet/ChotaChandraModel.obj");

	initializeStaticModel(&tree, "Media/Models/Hut/tree.obj");
	//initializeStaticModel(&chanakyaStanding, "Media/Models/chanakya model/chanakya holding stick.obj");

	//Bazier
	std::vector<std::vector<float>> points;
	points.push_back({ -205.302231,7.018716,-0.431156 });
	points.push_back({ 0.060963,16.606358,-0.985229 });
	points.push_back({ 128.540955,19.813559,11.39737 });

	// Cubemap
	if (cubeMapShader->initializeCubeMapShaderProgram() != 0) {
		return -1;
	}
	cubemapTexture = textures->loadCubeMapTexture(facesCubeMap);

	// texture model
	if (textureShader->initializeTextureShaderProgram() != 0) {
		return -1;
	}
	textures->storeTextureFromFile("Media\\Textures\\Test", "Stone.bmp", ID_BITMAP_STONE);

	// texture model
	if (textureShader->initializeTextureShaderProgram() != 0) {
		return -1;
	}
	textures->storeTextureFromFile("Media\\Textures\\Test", "Stone.bmp", ID_BITMAP_STONE);

	std::vector<float> yaw;
	yaw.push_back(1.601161);
	yaw.push_back(-0.798865);
	yaw.push_back(-173.198);


	std::vector<float> pitch;
	pitch.push_back(-1.999);
	pitch.push_back(-1.6000);
	pitch.push_back(-8.400013);

	bazierCameraForSceneTraining->initialize();
	bazierCameraForSceneTraining->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneTraining->time = 0;

	points.clear();
	yaw.clear();
	pitch.clear();


	points.push_back({ 128.540955,19.813559,11.39737 });
	points.push_back({ 75.649910,13.062579,11.383432 });

	yaw.push_back(-173.198);
	yaw.push_back(281.199890);


	pitch.push_back(-8.400013);
	pitch.push_back(-4.200023);

	bazierCameraForSceneTrainingTwo->initialize();
	bazierCameraForSceneTrainingTwo->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneTrainingTwo->time = 0;

	// Terrain initialization
	if (terrainShader->initializeTerrainShaderProgram("Media\\Textures\\Terrain\\heightmap.png") != 0) {
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

	return 0;
}

void Scene::displaySceneTraining() {
	mat4 modelMatrixArray[1];

	if (initCameraForSceneTraining == 0) {
		initCameraForSceneTraining = 1;
		freeCamera->position = vec3(0.0,0.0,0.0);
		freeCamera->front = vec3(0.999458, 0.027922, 0.017446);
		freeCamera->yaw = 1.000035;
		freeCamera->updateCameraVectors();
	}
	if (initSongForSceneTraining == 0) {
		if (AUDIO_ENABLE) {
			initSongForSceneTraining = 1;
			playSong(5);
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
		if (camNum == 0)
		{
			viewMatrix = bazierCameraForSceneTraining->getViewMatrix();
			camPos = bazierCameraForSceneTraining->getCameraPosition();
		}
		if (camNum == 1)
		{
			viewMatrix = bazierCameraForSceneTrainingTwo->getViewMatrix();
			camPos = bazierCameraForSceneTrainingTwo->position;
		}
	}

	modelDirectionLightStruct.directionLight_Direction = vec3(-50.600739, 50.00, 100.899);
	modelMatrixArray[0] = genrateModelMatrix(vec3(-52.199917, -1.200000, 94.50), vec3(0.600000, -171.000977, 0.00), vec3(12.700005, 12.700005, 12.700005));
	//modelMatrixArray[0] = genrateModelMatrix(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(5.900005, 5.900005, 5.90000));
	objmodelLoadingShader->displayObjModelLoadingShader(&seat, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);


	modelDirectionLightStruct.directionLight_Direction = vec3(-50.600739, 50.00, 100.899);
	modelMatrixArray[0] = genrateModelMatrix(vec3(-52.199917, -1.200000, 94.50), vec3(0.600000, -171.000977, 0.00), vec3(12.700005, 12.700005, 12.700005));
	//modelMatrixArray[0] = genrateModelMatrix(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(5.900005, 5.900005, 5.90000));
	objmodelLoadingShader->displayObjModelLoadingShader(&chandragupta, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelDirectionLightStruct.directionLight_Direction = vec3(-50.600739, 50.00, 100.899);
	modelMatrixArray[0] = genrateModelMatrix(vec3(-42.700001, 14.000002, 86.700), vec3(0.600000, 0.0, 0.00), vec3(0.400, 0.400, 0.400));
	//modelMatrixArray[0] = genrateModelMatrix(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(5.900005, 5.900005, 5.90000));
	objmodelLoadingShader->displayObjModelLoadingShader(&tree, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	terrainShader->useTerrainShaderProgram();
	terrainShader->lightPos = vec3(0.0, 0.0, 0.0);
	modelMatrix = genrateModelMatrix(transformationVector.translationVector, transformationVector.rotationVector, vec3(1.0,1.0,1.0));
	terrainShader->displayTerrainShader(terrainTextures, modelMatrix, viewMatrix, camPos, transformationVector.scaleVector[0], transformationVector.scaleVector[1]);
	terrainShader->unUseTerrainShaderProgram();

	//terrainShader->useTerrainShaderProgram();
	//terrainShader->lightPos = vec3(0.0, 0.0, 0.0);
	//modelMatrix = genrateModelMatrix(vec3(319.600006, -2.599983, 597.000732), vec3(0.000000, -171.000061, 0.000000), vec3(1.0));
	//terrainShader->displayTerrainShader(terrainTextures, modelMatrix, viewMatrix, camPos, 41.699921, 0.60000);
	//terrainShader->unUseTerrainShaderProgram();

	modelMatrix = genrateModelMatrix(vec3(0.0), vec3(0.0, -200.0, 0.0), vec3(500.0, 500.0, 500.0));
	cubeMapShader->useCubeMapShaderProgram();
	cubeMapShader->displayCubeMapShader(cubemapTexture, modelMatrix, viewMatrix);
	renderes->renderCube();
	cubeMapShader->unUseCubeMapShaderProgram();
	glBindTexture(GL_TEXTURE_2D, 0);

}
void Scene::updateSceneTraining() {
	if (!debugCamera)
	{
		if (bazierCameraForSceneTraining->time < 1.0f) {
			bazierCameraForSceneTraining->time += 0.00025f;
			bazierCameraForSceneTraining->update();
		}
		/*else
		{
			camNum = 1;
			if (bazierCameraForSceneTrainingTwo->time < 1.0f) {
				bazierCameraForSceneTrainingTwo->time += 0.0003f;
				bazierCameraForSceneTrainingTwo->update();
			}
		}*/
	}
	fireShader->setFrameTime(0.001);
}

void Scene::unitializeSceneTraining() {

	objmodelLoadingShader->deleteShaderProgramObject();
	bazierCameraForSceneTraining->uninitialize();


}
