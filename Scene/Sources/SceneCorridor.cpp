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
static StaticModel chandraStand;

static TerrainShader* terrainShader = new TerrainShader();

static int camNum = 0;

void Scene::WndProcForSceneCorridor(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {
	//code
}

int Scene::initialiseSceneCorridor() {
	TextureManager textureManager;
	glClearColor(0.0, 0.0, 0.0, 1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Atmosphere Initialization
	if (objmodelLoadingShader->initializeObjModelLoadingShaderProgram() != 0) {
		return -1;
	}
	initializeStaticModel(&palace, "Media/Models/CityWithCastle/city2.obj");
	initializeStaticModel(&corridor, "Media/Models/01- Ancient.Corridor/corridor_room.obj");
	initializeStaticModel(&chanakyaStanding, "Media/Models/chanakya model/chanakya holding stick.obj");
	//initializeStaticModel(&chandraStand, "Media/Models/_Share chandragupt/standingChandra.obj");


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
	points.push_back({ -216.913925,18.657698,3.244141 });
	points.push_back({ -135.600708,14.815137,0.216430 });
	//points.push_back({ 7.899680,18.901358,8.737787 });
	points.push_back({ 32.141411,18.119602,0.064979 });

	std::vector<float> yaw;
	yaw.push_back(-3.20048);
	yaw.push_back(-2.200482);
	//yaw.push_back(-20.80047);
	yaw.push_back(1.800087);

	std::vector<float> pitch;
	pitch.push_back(-8.600042);
	pitch.push_back(-2.200046);
	//pitch.push_back(-6.40006);
	pitch.push_back(0.199992);

	bazierCameraForSceneCorridor->initialize();
	bazierCameraForSceneCorridor->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneCorridor->time = 0;

	points.clear();
	yaw.clear();
	pitch.clear();


	points.push_back({ 32.141411,18.119602,0.064979 });
	points.push_back({ 85.810799,10.605152,-49.677536 });
	points.push_back({ 128.499931,22.443741,-0.500428 });
	points.push_back({ 80.623650,20.647453,43.773724 });
	points.push_back({ 75.973984,13.605128,13.857044 });

	yaw.push_back(1.800087);
	yaw.push_back(91.600090);
	yaw.push_back(178.199966);
	yaw.push_back(270.399994);
	yaw.push_back(280.600067);

	pitch.push_back(0.199992);
	pitch.push_back(0.599993);
	pitch.push_back(-9.799995);
	pitch.push_back(-7.599997);
	pitch.push_back(-14.799992);


	bazierCameraForSceneCorridorTwo->initialize();
	bazierCameraForSceneCorridorTwo->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneCorridorTwo->time = 0;

	return 0;
}

void Scene::displaySceneCorridor() {
	mat4 modelMatrixArray[1];
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

	modelPointLightStruct[0].pointLight_position = vec3(84.600098, 14.100006, 12.300005);
	numOfPointLight = 1;
	modelDirectionLightStruct.directionLight_Direction = vec3(0.600739, 5000.00, 0.899);
	modelMatrixArray[0] = genrateModelMatrix(vec3(78.200157, 11.900004, 2.3), vec3(0.0, -90.0, 0.0), vec3(0.030));
	objmodelLoadingShader->displayObjModelLoadingShader(&palace, modelMatrixArray, viewMatrix, 1, MODEL_POINTLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelDirectionLightStruct.directionLight_Direction = vec3(0.600739, 50.00, 0.899);
	modelMatrixArray[0] = genrateModelMatrix(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(5.900005, 5.900005, 5.90000));

	modelPointLightStruct[0].pointLight_position = transformationVector.translationVector;

	modelPointLightStruct[0].pointLight_position = vec3(11.5, 24.0, -13.0);
	modelPointLightStruct[1].pointLight_position = vec3(-16.0, 24.0, 13.5);
	modelPointLightStruct[2].pointLight_position = vec3(-43.5, 24.0, -13.5);
	modelPointLightStruct[3].pointLight_position = vec3(-71.0, 24.0, 13.5);
	modelPointLightStruct[4].pointLight_position = vec3(-99.500000, 24.0, -13.500000);
	modelPointLightStruct[5].pointLight_position = vec3(-127.500000, 24.0, 13.000000);
	modelPointLightStruct[6].pointLight_position = vec3(-155.0, 24.0, -13.500000);
	modelPointLightStruct[7].pointLight_position = vec3(-183.000000, 24.0, 13.500000);

	modelPointLightStruct[8].pointLight_position = vec3(124.500000, 29.699987, 2.000000);
	modelPointLightStruct[9].pointLight_position = vec3(106.0, 30.2, -39.0);
	modelPointLightStruct[10].pointLight_position = vec3(51.0, 29.2, -38.0);
	modelPointLightStruct[11].pointLight_position = vec3(54.000000, 29.699987, 41.000000);
	modelPointLightStruct[12].pointLight_position = vec3(102.000000, 30.699987, 40.000000);

	modelPointLightStruct[13].pointLight_position = vec3(79.000000, 18.699987, -0.500000);
	//modelPointLightStruct[14].pointLight_position = vec3(84.600098, 14.100006, 12.300005);
	numOfPointLight = 14;

	for (int i = 0; i < numOfPointLight; i++)
	{
		 modelPointLightStruct[i].pointLight_ambient = vec3(0.1f, 0.1f, 0.1f);
		 modelPointLightStruct[i].pointLight_diffuse = vec3(0.25f, 0.25f, 0.0f);
		 modelPointLightStruct[i].pointLight_specular = vec3(0.1f, 0.1f, 0.1f);
		 modelPointLightStruct[i].pointLight_constant = 1.0f;
		 modelPointLightStruct[i].pointLight_linear = 0.07000;
		 modelPointLightStruct[i].pointLight_quadratic = 0.00090;
	}

	objmodelLoadingShader->displayObjModelLoadingShader(&corridor, modelMatrixArray, viewMatrix, 1, MODEL_POINTLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelPointLightStruct[0].pointLight_position = vec3(84.600098, 14.100006, 12.300005);
	numOfPointLight = 1;

	modelDirectionLightStruct.directionLight_Direction = vec3(0.600739, 50.00, 0.899);
	//modelMatrixArray[0] = genrateModelMatrix(transformationVector.translationVector, transformationVector.rotationVector, transformationVector.scaleVector);
	modelMatrixArray[0] = genrateModelMatrix(vec3(75.300079, 0.000000, 53.999977), vec3(0.000000, -119.199890, 0.000000), vec3(11.300003, 11.300003, 11.300));
	objmodelLoadingShader->displayObjModelLoadingShader(&chanakyaStanding, modelMatrixArray, viewMatrix, 1, MODEL_POINTLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	//modelDirectionLightStruct.directionLight_Direction = vec3(0.600739, 50.00, 0.899);
	//modelMatrixArray[0] = genrateModelMatrix(transformationVector.translationVector, transformationVector.rotationVector, transformationVector.scaleVector);
	////modelMatrixArray[0] = genrateModelMatrix(vec3(75.300079, 0.000000, 53.999977), vec3(0.000000, -119.199890, 0.000000), vec3(11.300003, 11.300003, 11.300));
	//objmodelLoadingShader->displayObjModelLoadingShader(&chandraStand, modelMatrixArray, viewMatrix, 1, MODEL_POINTLIGHT);
	//glBindTexture(GL_TEXTURE_2D, 0);


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
			bazierCameraForSceneCorridor->time += 0.00045f;
			bazierCameraForSceneCorridor->update();
		}
		else
		{
			camNum = 1;
			if (bazierCameraForSceneCorridorTwo->time < 1.0f) {
				bazierCameraForSceneCorridorTwo->time += 0.0020f;
				bazierCameraForSceneCorridorTwo->update();
			}
		}
	}
	fireShader->setFrameTime(0.001);
}

void Scene::unitializeSceneCorridor() {

	objmodelLoadingShader->deleteShaderProgramObject();
	bazierCameraForSceneCorridor->uninitialize();


}
