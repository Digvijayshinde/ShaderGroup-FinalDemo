#include"../../Source/Globals.h"
#include "../../Scene/Headers/Scene.h"
#include "../../Shaders/ObjModelLoading/ObjModelLoading.h"
#include "../../Utility/Headers/ModelLoading/StaticModelLoader.h"
#include "../../Shaders/CommomTexture/Texture.h"
#include "../../Utility/Headers/AudioPlayer.h"
#include "../../../Utility/Headers/BezierCamera.h"
#include "vector"
#include"../../Shaders/Fire/Fire.h"
#include "../../Shaders/Terrain/Terrain.h"

static TextureShader* textureShader = new TextureShader();
static FireShader* fireShader = new FireShader();

static ObjModelLoadingShader* objmodelLoadingShader = new ObjModelLoadingShader();;

static int initCameraForSceneCorridor = 0;
static int initSongForSceneCorridor = 0;

static BezierCamera* bazierCameraForSceneCorridor = new BezierCamera();
static BezierCamera* bazierCameraForSceneCorridorTwo = new BezierCamera();

static TextureManager* textures = new TextureManager();
static TextureManager* terrainTextures = new TextureManager();

static StaticModel corridor;
static StaticModel smallCity;

extern StaticModel palace;

extern StaticModel chanakyaStanding;
static TerrainShader* terrainShader = new TerrainShader();

static int camNum = 0;

void Scene::WndProcForSceneCorridor(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {
	//code
}

int Scene::initialiseSceneCorridor() {
	TextureManager textureManager;

	// Atmosphere Initialization
	if (objmodelLoadingShader->initializeObjModelLoadingShaderProgram() != 0) {
		return -1;
	}
	initializeStaticModel(&palace, "Media/Models/CityWithCastle/city2.obj");

	initializeStaticModel(&corridor, "Media/Models/01- Ancient.Corridor/corridor_room.obj");
	//initializeStaticModel(&smallCity, "Media/Models/smallCity/base.obj");

	initializeStaticModel(&chanakyaStanding, "Media/Models/chanakya model/chanakya holding stick.obj");

	textures->storeTextureFromFile("Media\\Textures\\Test", "Stone.bmp", ID_BITMAP_STONE);

	// Fire
	if (fireShader->initializeFireShaderProgram() == -1)
		return -1;
	//clipping is requid for fire 
	isDDSTextureClipped = true;
	textures->storeTextureFromFile("Media\\Textures\\Fire", "fire.dds");
	textures->storeTextureFromFile("Media\\Textures\\Fire", "alpha.dds");
	isDDSTextureClipped = false;
	textures->storeTextureFromFile("Media\\Textures\\Fire", "noise.dds");


	//Bazier
	std::vector<std::vector<float>> points;
	points.push_back({ -205.302231,7.018716,-0.431156 });
	points.push_back({ 0.060963,16.606358,-0.985229 });
	points.push_back({ 128.540955,19.813559,11.39737 });

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

	bazierCameraForSceneCorridor->initialize();
	bazierCameraForSceneCorridor->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneCorridor->time = 0;

	points.clear();
	yaw.clear();
	pitch.clear();


	points.push_back({ 128.540955,19.813559,11.39737 });
	points.push_back({ 75.649910,13.062579,11.383432 });

	yaw.push_back(-173.198);
	yaw.push_back(281.199890);


	pitch.push_back(-8.400013);
	pitch.push_back(-4.200023);

	bazierCameraForSceneCorridorTwo->initialize();
	bazierCameraForSceneCorridorTwo->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneCorridorTwo->time = 0;

	return 0;
}

void Scene::displaySceneCorridor() {
	mat4 modelMatrixArray[1];

	if (initCameraForSceneCorridor == 0) {
		initCameraForSceneCorridor = 1;
		freeCamera->position = vec3(182.445862, 17.948416, 243.554276);
		freeCamera->front = vec3(0.999458, 0.027922, 0.017446);
		freeCamera->yaw = 1.000035;
		freeCamera->updateCameraVectors();
	}
	if (initSongForSceneCorridor == 0) {
		if (AUDIO_ENABLE) {
			initSongForSceneCorridor = 1;
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
			viewMatrix = bazierCameraForSceneCorridor->getViewMatrix();
			camPos = bazierCameraForSceneCorridor->getCameraPosition();
		}
		if (camNum == 1)
		{
			viewMatrix = bazierCameraForSceneCorridorTwo->getViewMatrix();
			camPos = bazierCameraForSceneCorridorTwo->position;
		}
	}

	modelDirectionLightStruct.directionLight_Direction = vec3(0.600739, 5000.00, 0.899);
	modelMatrixArray[0] = genrateModelMatrix(vec3(78.200157, 11.900004, 2.3), vec3(0.0,-90.0,0.0), vec3(0.030));
	objmodelLoadingShader->displayObjModelLoadingShader(&palace, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelDirectionLightStruct.directionLight_Direction = vec3(0.600739, 50.00, 0.899);
	modelMatrixArray[0] = genrateModelMatrix(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(5.900005, 5.900005, 5.90000));
	objmodelLoadingShader->displayObjModelLoadingShader(&corridor, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelDirectionLightStruct.directionLight_Direction = vec3(0.600739, 50.00, 0.899);
	//modelMatrixArray[0] = genrateModelMatrix(transformationVector.translationVector, transformationVector.rotationVector, transformationVector.scaleVector);
	modelMatrixArray[0] = genrateModelMatrix(vec3(75.300079, 0.000000, 53.999977), vec3(0.000000, -119.199890, 0.000000), vec3(11.300003, 11.300003, 11.300));
	objmodelLoadingShader->displayObjModelLoadingShader(&chanakyaStanding, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);


	modelMatrix = genrateModelMatrix(vec3(-155,25.30,-12.40), vec3(0.00, 70.799911, -183.599838), vec3(1.80));
	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, textures->getTexture("alpha"), textures->getTexture("fire"), textures->getTexture("noise"));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	renderes->renderQuad();

	modelMatrix = genrateModelMatrix(vec3(-100, 25.30, -12.40), vec3(0.00, 70.799911, -183.599838), vec3(1.80));
	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, textures->getTexture("alpha"), textures->getTexture("fire"), textures->getTexture("noise"));
	renderes->renderQuad();

	modelMatrix = genrateModelMatrix(vec3(-44.40, 25.30,-12.40), vec3(0.00, 70.799911, -183.599838), vec3(1.80));
	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, textures->getTexture("alpha"), textures->getTexture("fire"), textures->getTexture("noise"));
	renderes->renderQuad();

	modelMatrix = genrateModelMatrix(vec3(11.20, 25.30, -12.40), vec3(0.00, 70.799911, -183.599838), vec3(1.80));
	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, textures->getTexture("alpha"), textures->getTexture("fire"), textures->getTexture("noise"));
	renderes->renderQuad();

	modelMatrix = genrateModelMatrix(vec3(-16.80, 25.30, 12.70), vec3(0.00, 70.799911, -183.599838), vec3(1.80));
	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, textures->getTexture("alpha"), textures->getTexture("fire"), textures->getTexture("noise"));
	renderes->renderQuad();

	modelMatrix = genrateModelMatrix(vec3(-72.10, 25.30, 12.70), vec3(0.00, 70.799911, -183.599838), vec3(1.80));
	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, textures->getTexture("alpha"), textures->getTexture("fire"), textures->getTexture("noise"));
	renderes->renderQuad();

	modelMatrix = genrateModelMatrix(vec3(-127.50, 25.30, 12.70), vec3(0.00, 70.799911, -183.599838), vec3(1.80));
	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, textures->getTexture("alpha"), textures->getTexture("fire"), textures->getTexture("noise"));
	renderes->renderQuad();

	modelMatrix = genrateModelMatrix(vec3(-183.30, 25.30, 12.70), vec3(0.00, 70.799911, -183.599838), vec3(1.80));
	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, textures->getTexture("alpha"), textures->getTexture("fire"), textures->getTexture("noise"));
	renderes->renderQuad();

	modelMatrix = genrateModelMatrix(vec3(54.700302, 29.500006, 41.300003), vec3(0.000000, 99.799950, -183.599838), vec3(1.80));
	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, textures->getTexture("alpha"), textures->getTexture("fire"), textures->getTexture("noise"));
	renderes->renderQuad();

	modelMatrix = genrateModelMatrix(vec3(102.30, 30.20, 39.90), vec3(0.000000, 145.299759, -183.599838), vec3(1.80));
	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, textures->getTexture("alpha"), textures->getTexture("fire"), textures->getTexture("noise"));
	renderes->renderQuad();

	modelMatrix = genrateModelMatrix(vec3(105.80, 29.50, -39.90), vec3(0.000000, 145.299759, -183.599838), vec3(1.80));
	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, textures->getTexture("alpha"), textures->getTexture("fire"), textures->getTexture("noise"));
	renderes->renderQuad();

	modelMatrix = genrateModelMatrix(vec3(51.200306, 28.100004, -37.800041), vec3(0.000000, 40.300087, -183.599838), vec3(1.80));
	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, textures->getTexture("alpha"), textures->getTexture("fire"), textures->getTexture("noise"));
	renderes->renderQuad();

	modelMatrix = genrateModelMatrix(vec3(126.800049, 28.800005, 2.099979), vec3(0.000000, 265.699432, -183.599838), vec3(1.80));
	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, textures->getTexture("alpha"), textures->getTexture("fire"), textures->getTexture("noise"));
	renderes->renderQuad();
	fireShader->unUseFireShaderProgram();
	glDisable(GL_BLEND);

}
void Scene::updateSceneCorridor() {
	if (!debugCamera)
	{
		if (bazierCameraForSceneCorridor->time < 1.0f) {
			bazierCameraForSceneCorridor->time += 0.00025f;
			bazierCameraForSceneCorridor->update();
		}
		/*else
		{
			camNum = 1;
			if (bazierCameraForSceneCorridorTwo->time < 1.0f) {
				bazierCameraForSceneCorridorTwo->time += 0.0003f;
				bazierCameraForSceneCorridorTwo->update();
			}
		}*/
	}
	fireShader->setFrameTime(0.001);
}

void Scene::unitializeSceneCorridor() {

	objmodelLoadingShader->deleteShaderProgramObject();
	bazierCameraForSceneCorridor->uninitialize();


}
