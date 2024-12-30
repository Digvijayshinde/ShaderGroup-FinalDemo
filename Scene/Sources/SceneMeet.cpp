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

static int initCameraForSceneMeet = 0;
static int initSongForSceneMeet = 0;

static BezierCamera* bazierCameraForSceneMeet = new BezierCamera();
static BezierCamera* bazierCameraForSceneMeetTwo = new BezierCamera();

static TextureManager* textures = new TextureManager();
static TextureManager* terrainTextures = new TextureManager();

static StaticModel corridor;
static StaticModel smallCity;

extern StaticModel chanakyaStanding;
static TerrainShader* terrainShader = new TerrainShader();

static int camNum = 0;

void Scene::WndProcForSceneMeet(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {
	//code
}

int Scene::initialiseSceneMeet() {
	TextureManager textureManager;

	// Atmosphere Initialization
	if (objmodelLoadingShader->initializeObjModelLoadingShaderProgram() != 0) {
		return -1;
	}
	initializeStaticModel(&corridor, "Media/Models/01- Ancient.Corridor/corridor_room.obj");
	initializeStaticModel(&smallCity, "Media/Models/smallCity/base.obj");

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

	bazierCameraForSceneMeet->initialize();
	bazierCameraForSceneMeet->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneMeet->time = 0;

	points.clear();
	yaw.clear();
	pitch.clear();


	points.push_back({ 128.540955,19.813559,11.39737 });
	points.push_back({ 75.649910,13.062579,11.383432 });

	yaw.push_back(-173.198);
	yaw.push_back(281.199890);


	pitch.push_back(-8.400013);
	pitch.push_back(-4.200023);

	bazierCameraForSceneMeetTwo->initialize();
	bazierCameraForSceneMeetTwo->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneMeetTwo->time = 0;

	return 0;
}

void Scene::displaySceneMeet() {
	mat4 modelMatrixArray[1];

	if (initCameraForSceneMeet == 0) {
		initCameraForSceneMeet = 1;
		freeCamera->position = vec3(182.445862, 17.948416, 243.554276);
		freeCamera->front = vec3(0.999458, 0.027922, 0.017446);
		freeCamera->yaw = 1.000035;
		freeCamera->updateCameraVectors();
	}
	if (initSongForSceneMeet == 0) {
		if (AUDIO_ENABLE) {
			initSongForSceneMeet = 1;
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
			viewMatrix = bazierCameraForSceneMeet->getViewMatrix();
			camPos = bazierCameraForSceneMeet->getCameraPosition();
		}
		if (camNum == 1)
		{
			viewMatrix = bazierCameraForSceneMeetTwo->getViewMatrix();
			camPos = bazierCameraForSceneMeetTwo->position;
		}
	}

	modelDirectionLightStruct.directionLight_Direction = vec3(0.600739, 50.00, 0.899);
	modelMatrixArray[0] = genrateModelMatrix(vec3(78.50, 10.60, -2.30), vec3(-2.400000, -0.200000, 0.00), vec3(4.80, 4.80, 4.80));
	objmodelLoadingShader->displayObjModelLoadingShader(&smallCity, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelDirectionLightStruct.directionLight_Direction = vec3(0.600739, 50.00, 0.899);
	modelMatrixArray[0] = genrateModelMatrix(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(5.900005, 5.900005, 5.90000));
	objmodelLoadingShader->displayObjModelLoadingShader(&corridor, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3(-155, 25.30, -12.40), vec3(0.00, 70.799911, -183.599838), vec3(1.80));
	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, textures->getTexture("alpha"), textures->getTexture("fire"), textures->getTexture("noise"));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	renderes->renderQuad();
	glDisable(GL_BLEND);

}
void Scene::updateSceneMeet() {
	if (!debugCamera)
	{
		if (bazierCameraForSceneMeet->time < 1.0f) {
			bazierCameraForSceneMeet->time += 0.00025f;
			bazierCameraForSceneMeet->update();
		}
		/*else
		{
			camNum = 1;
			if (bazierCameraForSceneMeetTwo->time < 1.0f) {
				bazierCameraForSceneMeetTwo->time += 0.0003f;
				bazierCameraForSceneMeetTwo->update();
			}
		}*/
	}
	fireShader->setFrameTime(0.001);
}

void Scene::unitializeSceneMeet() {

	objmodelLoadingShader->deleteShaderProgramObject();
	bazierCameraForSceneMeet->uninitialize();


}
