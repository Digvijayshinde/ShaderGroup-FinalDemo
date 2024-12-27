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
static StaticModel palace;
static StaticModel chanakya;
static StaticModel dhananand;


static ObjModelLoadingShader* objmodelLoadingShader = new ObjModelLoadingShader();
static TextureShader* textureShader = new TextureShader();
static TextureManager* textures = new TextureManager();
static int initCameraForSceneOpening = 0;
static int initSongForSceneOpening = 0;

static BezierCamera* bazierCameraForSceneOpening = new BezierCamera();

void Scene::WndProcForSceneOpening(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {
	//code
}

int Scene::initialiseSceneOpening() {
	// Atmosphere Initialization
	if (atmosphericScatteringShader->initializeAtmosphericScatteringProgram() == -1)
		return -1;

	if (objmodelLoadingShader->initializeObjModelLoadingShaderProgram() != 0) {
		return -1;
	}
	initializeStaticModel(&palace, "Media/Models/Palace/untitled.obj");
	initializeStaticModel(&chanakya, "Media/Models/chanakyaTemp/chanakyaTemp.obj");
	initializeStaticModel(&dhananand, "Media/Models/Dhananand/dhananand.obj");



	if (textureShader->initializeTextureShaderProgram() != 0) {
		return -1;
	}
	textures->storeTextureFromFile("Media\\Textures\\Test", "Stone.bmp", ID_BITMAP_STONE);

	//Bazier
	std::vector<std::vector<float>> points;
	points.push_back({ 161.158997,10016.964844,144.451477 });
	points.push_back({ 203.903061,10017.529297,175.925552 });
	points.push_back({ 174.875793,10019.057617,213.723190 });
	points.push_back({ 141.431335,10014.075195,165.468811 });
	//points.push_back({ 139.737900,10016.585938,167.811462 });

	std::vector<float> yaw;
	yaw.push_back(47.000175);
	yaw.push_back(69.600159);
	yaw.push_back(5.000071);
	yaw.push_back(32.600090);
	//yaw.push_back(25.79);


	std::vector<float> pitch;
	pitch.push_back(-0.400046);
	pitch.push_back(1.799954);
	pitch.push_back(-0.400046);
	pitch.push_back(-0.400062);
	//pitch.push_back(-0.800019);

	bazierCameraForSceneOpening->initialize();
	bazierCameraForSceneOpening->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneOpening->time = 0;

	return 0;
}

void Scene::displaySceneOpening() {

	if (initCameraForSceneOpening == 0) {
		initCameraForSceneOpening = 1;
		freeCamera->position = vec3(182.445862,10000+17.948416, 243.554276);
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
		viewMatrix = bazierCameraForSceneOpening->getViewMatrix();
		camPos = bazierCameraForSceneOpening->getCameraPosition();
	}

	modelMatrix = genrateModelMatrix(vec3(261.800140, 0.0, 269.100159), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	atmosphericScatteringShader->useGroundAtmosphericScatteringProgram();
	atmosphericScatteringShader->displayGroundAtmosphericScatteringShader(modelMatrix,viewMatrix,camPos);
	atmosphericScatteringShader->unUseGroundAtmosphericScatteringProgram();
	atmosphericScatteringShader->useSkyAtmosphericScatteringProgram();
	atmosphericScatteringShader->displaySkyAtmosphericScatteringShader(modelMatrix,viewMatrix,camPos);
	atmosphericScatteringShader->unUseSkyAtmosphericScatteringProgram();

	modelMatrix = genrateModelMatrix(vec3(247.200897, 10000+16.80, 249.101501f), vec3(0.000000, -128.000000, 0.000000), vec3(0.300000, 0.300000, 0.300000));
	modelPointLightStruct[0].pointLight_position = vec3(223.199829, 19.199 + 10000, 220.79);
	modelPointLightStruct[1].pointLight_position = vec3(196.199829, 15.399 + 10000, 200.199);
	modelPointLightStruct[2].pointLight_position = vec3(180.199829, 15.399 + 10000, 186.199);
	modelPointLightStruct[3].pointLight_position = vec3(159.000015, 16.500002 + 10000, 171.000015);
	modelPointLightStruct[4].pointLight_position = vec3(212.099731, 25.200003 + 10000, 212.799728);
	modelPointLightStruct[5].pointLight_position = vec3(transformationVector.translationVector[0], transformationVector.translationVector[1] + 10000, transformationVector.translationVector[2]);
	numOfPointLight = 6;
	objmodelLoadingShader->displayObjModelLoadingShader(&palace, modelMatrix, viewMatrix, MODEL_POINTLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3(247.200897 + (-91.20), 10000 + 10.200, 249.101501f + (-75.30)), vec3(0.000000, 16.200006, 0.000000), vec3(0.100001, 0.100001, 0.100001));
	objmodelLoadingShader->displayObjModelLoadingShader(&chanakya, modelMatrix, viewMatrix, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3(247.200897 + -25.899933, 10000 + 18.000006, 249.101501f - 29.799931), vec3(.000000, -126.099907, 0.000000), vec3(5.200000, 5.200000, 5.200000));
	objmodelLoadingShader->displayObjModelLoadingShader(&dhananand, modelMatrix, viewMatrix, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3(transformationVector.translationVector[0], transformationVector.translationVector[1]+10000, transformationVector.translationVector[2]), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	textureShader->useTextureShaderProgram();
	textureShader->displayTextureShader(modelMatrix, viewMatrix, textures->getTexture("Stone"), 0);
	renderes->renderSphere();
	textureShader->unUseTextureShaderProgram();

	glBindTexture(GL_TEXTURE_2D, 0);
}
void Scene::updateSceneOpening() {
	if (!debugCamera)
	{
		if (bazierCameraForSceneOpening->time < 1.0f) {
			bazierCameraForSceneOpening->time += 0.00015f;
			bazierCameraForSceneOpening->update();
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
