#include"../../Source/Globals.h"
#include "../../Scene/Headers/Scene.h"
#include "../../Shaders/AtmosphericScattering/AtmosphericScattering.h"
#include "../../Shaders/Terrain/Terrain.h"
#include "../../Shaders/ObjModelLoading/ObjModelLoading.h"
#include "../../Utility/Headers/ModelLoading/StaticModelLoader.h"
#include "../../Shaders/CommomTexture/Texture.h"
#include "../../Utility/Headers/AudioPlayer.h"
#include "../../../Utility/Headers/BezierCamera.h"

static AtmosphericScatteringShader* atmosphericScatteringShader = new AtmosphericScatteringShader();
static StaticModel chanakya;
static StaticModel dhananand;


static ObjModelLoadingShader* objmodelLoadingShader = new ObjModelLoadingShader();
static TextureShader* textureShader = new TextureShader();
static TextureManager* textures = new TextureManager();
static int initCameraForSceneOpening = 0;
static int initSongForSceneOpening = 0;

static BezierCamera* bazierCameraForSceneOpening = new BezierCamera();
static BezierCamera* bazierCameraForSceneOpeningTwo = new BezierCamera();

extern StaticModel palaceInterior;
static int camNum = 0;

void Scene::WndProcForSceneOpening(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {
	//code
}

int Scene::initialiseSceneOpening() {
	// Atmosphere Initialization
	if (objmodelLoadingShader->initializeObjModelLoadingShaderProgram() != 0) {
		return -1;
	}
	initializeStaticModel(&palaceInterior, "Media/Models/palace/palace_latest10.obj");

	initializeStaticModel(&chanakya, "Media/Models/chanakya model/chanakya giving direction.obj");
	//initializeStaticModel(&dhananand, "Media/Models/Dhananand/dhananand.obj");


	if (textureShader->initializeTextureShaderProgram() != 0) {
		return -1;
	}
	textures->storeTextureFromFile("Media\\Textures\\Test", "Stone.bmp", ID_BITMAP_STONE);

	//Bazier
	std::vector<std::vector<float>> points;
	points.push_back({ 177.561890,21.087084,217.767197 });
	points.push_back({ 183.499512,21.087084,190.587784 });
	points.push_back({ 207.755981,21.099232,176.310257 });

	points.push_back({ 216.910233,18.030783,200.055420 });
	points.push_back({ 208.935974,16.738047,215.168167 });
	//points.push_back({ 139.737900,10016.585938,167.811462 });

	std::vector<float> yaw;
	yaw.push_back(-1.399949);
	yaw.push_back(37.600079);
	yaw.push_back(79.599991);
	yaw.push_back(100.3998261);
	yaw.push_back(220.599655);
	//yaw.push_back(25.79);


	std::vector<float> pitch;
	pitch.push_back(-9.600064);
	pitch.push_back(-11.800062);
	pitch.push_back(-12.400062);
	pitch.push_back(-7.400064);
	pitch.push_back(-2.800096);
	//pitch.push_back(-0.800019);

	bazierCameraForSceneOpening->initialize();
	bazierCameraForSceneOpening->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneOpening->time = 0;

	points.clear();
	yaw.clear();
	pitch.clear();
	//-------------------------------------------

	points.push_back({ 191.428802,18.571873,195.78872 });
	points.push_back({ 180.651276,14.022084,186.434845 });
	points.push_back({ 180.651276,14.022084,186.434845 });
	points.push_back({ 173.208832,13.707849,181.376862 });
	points.push_back({ 171.311356,13.707849,171.054535 });
	points.push_back({ 163.574524,15.482121,163.632584 });

	yaw.push_back(37.800041);
	yaw.push_back(37.800041);
	yaw.push_back(-146.199554);
	yaw.push_back(-146.799545);
	yaw.push_back(-260.799347);
	yaw.push_back(-290.799347);

	pitch.push_back(-5.400003);
	pitch.push_back(-2.800000);
	pitch.push_back(0.400000);
	pitch.push_back(4.400033);
	pitch.push_back(-8.799970);

	bazierCameraForSceneOpeningTwo->initialize();
	bazierCameraForSceneOpeningTwo->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneOpeningTwo->time = 0;

	return 0;
}

void Scene::displaySceneOpening() {
	mat4 modelMatrixArray[1];
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (initCameraForSceneOpening == 0) {
		initCameraForSceneOpening = 1;
		freeCamera->position = vec3(182.445862,17.948416, 243.554276);
		freeCamera->front = vec3(0.999458, 0.027922, 0.017446);
		freeCamera->yaw = 1.000035;
		freeCamera->updateCameraVectors();
	}
	if (initSongForSceneOpening == 0) {
		if (AUDIO_ENABLE) {
			initSongForSceneOpening = 1;
			playSong(0);
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
			viewMatrix = bazierCameraForSceneOpening->getViewMatrix();
			camPos = bazierCameraForSceneOpening->getCameraPosition();
		}
		if (camNum == 1)
		{
			viewMatrix = bazierCameraForSceneOpeningTwo->getViewMatrix();
			camPos = bazierCameraForSceneOpeningTwo->position;
		}
	}

	//modelMatrix = genrateModelMatrix(vec3(261.800140, 0.0, 269.100159), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	//atmosphericScatteringShader->useGroundAtmosphericScatteringProgram();
	//atmosphericScatteringShader->displayGroundAtmosphericScatteringShader(modelMatrix,viewMatrix,camPos);
	//atmosphericScatteringShader->unUseGroundAtmosphericScatteringProgram();
	//atmosphericScatteringShader->useSkyAtmosphericScatteringProgram();
	//atmosphericScatteringShader->displaySkyAtmosphericScatteringShader(modelMatrix,viewMatrix,camPos);
	//atmosphericScatteringShader->unUseSkyAtmosphericScatteringProgram();

	modelMatrixArray[0] = genrateModelMatrix(vec3(247.200897, 16.80, 249.101501f), vec3(0.000000, -128.000000, 0.000000), vec3(0.300000, 0.300000, 0.300000));
	modelPointLightStruct[0].pointLight_position = vec3(223.199829, 19.199, 220.79);
	modelPointLightStruct[1].pointLight_position = vec3(196.199829, 15.399, 200.199);
	modelPointLightStruct[2].pointLight_position = vec3(180.199829, 15.399, 186.199);
	modelPointLightStruct[3].pointLight_position = vec3(159.000015, 16.500002, 171.000015);
	//modelPointLightStruct[4].pointLight_position = vec3(212.099731, 25.200003, 212.799728);
	modelPointLightStruct[4].pointLight_position = vec3(167.999954, 19.200001, 175.199936);

	modelPointLightStruct[5].pointLight_position = vec3(transformationVector.translationVector[0], transformationVector.translationVector[1], transformationVector.translationVector[2]);
	numOfPointLight = 6;
	objmodelLoadingShader->displayObjModelLoadingShader(&palaceInterior, modelMatrixArray, viewMatrix,1, MODEL_POINTLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);


	modelDirectionLightStruct.directionLight_Direction = vec3(150, 50, 150);
	modelMatrixArray[0] = genrateModelMatrix(vec3(166.299911, 9.499997, 176.39), vec3(2.899999, -98.899818, 179.79), vec3(-3.30, -3.30, -3.30));
	objmodelLoadingShader->displayObjModelLoadingShader(&chanakya, modelMatrixArray, viewMatrix, 1, MODEL_POINTLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	//modelMatrix = genrateModelMatrix(vec3(transformationVector.translationVector[0], transformationVector.translationVector[1], transformationVector.translationVector[2]), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	//textureShader->useTextureShaderProgram();
	//textureShader->displayTextureShader(modelMatrix, viewMatrix, textures->getTexture("Stone"), 0);
	//renderes->renderSphere();
	//textureShader->unUseTextureShaderProgram();
	//glBindTexture(GL_TEXTURE_2D, 0);
}
void Scene::updateSceneOpening() {
	if (!debugCamera)
	{
		if (bazierCameraForSceneOpening->time < 1.0f) {
			bazierCameraForSceneOpening->time += 0.00085f;
			bazierCameraForSceneOpening->update();
		}
		else
		{
			camNum = 1;
			if (bazierCameraForSceneOpeningTwo->time < 1.0f) {
				bazierCameraForSceneOpeningTwo->time += 0.00055f;
				bazierCameraForSceneOpeningTwo->update();
			}
		}
	}
}
void Scene::unitializeSceneOpening() {

	atmosphericScatteringShader->deleteShaderProgramObject();
	if (atmosphericScatteringShader)
		delete atmosphericScatteringShader;

	objmodelLoadingShader->deleteShaderProgramObject();
	bazierCameraForSceneOpening->uninitialize();
}
