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
extern StaticModel chanakyaStanding;
static StaticModel dhananandKneel;


static ObjModelLoadingShader* objmodelLoadingShader = new ObjModelLoadingShader();
static TextureShader* textureShader = new TextureShader();
static TextureManager* textures = new TextureManager();
static int initCameraForSceneFinal = 0;
static int initSongForSceneFinal = 0;

static BezierCamera* bazierCameraForSceneFinal = new BezierCamera();

extern StaticModel palaceInterior;

void Scene::WndProcForSceneFinal(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {
	//code
}

int Scene::initialiseSceneFinal() {
	mat4 modelMatrixArray[1];

	if (objmodelLoadingShader->initializeObjModelLoadingShaderProgram() != 0) {
		return -1;
	}
	initializeStaticModel(&palaceInterior, "Media/Models/palace/untitled.obj");
	//initializeStaticModel(&chanakyaStanding, "Media/Models/chanakya model/chanakya holding stick.obj");
	initializeStaticModel(&dhananandKneel, "Media/Models/dhananand_kneel/base.obj");


	if (textureShader->initializeTextureShaderProgram() != 0) {
		return -1;
	}
	textures->storeTextureFromFile("Media\\Textures\\Test", "Stone.bmp", ID_BITMAP_STONE);

	//Bazier
	std::vector<std::vector<float>> points;
	points.push_back({ 144.098907,15.166224,187.32 });
	points.push_back({ 183.520325,15.480574,214.4115 });
	points.push_back({ 205.520325,15.480574,217.4115 });

	points.push_back({ 217.925262,18.665340,220.58462 });
	points.push_back({ 174.387054,15.321909,182.64556 });
	//points.push_back({ 139.737900,10016.585938,167.811462 });

	std::vector<float> yaw;
	yaw.push_back(18.1999);
	yaw.push_back(0.79997);
	yaw.push_back(0.79997);
	yaw.push_back(-139.3991);
	yaw.push_back(-143.598953);
	//yaw.push_back(25.79);


	std::vector<float> pitch;
	pitch.push_back(0.400063);
	pitch.push_back(-1.5999);
	pitch.push_back(-1.5999);
	pitch.push_back(-2.599);
	pitch.push_back(-7.799);
	//pitch.push_back(-0.800019);

	bazierCameraForSceneFinal->initialize();
	bazierCameraForSceneFinal->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneFinal->time = 0;

	return 0;
}

void Scene::displaySceneFinal() {
	mat4 modelMatrixArray[1];

	if (initCameraForSceneFinal == 0) {
		initCameraForSceneFinal = 1;
		freeCamera->position = vec3(182.445862, 17.948416, 243.554276);
		freeCamera->front = vec3(0.999458, 0.027922, 0.017446);
		freeCamera->yaw = 1.000035;
		freeCamera->updateCameraVectors();
	}
	if (initSongForSceneFinal == 0) {
		if (AUDIO_ENABLE) {
			initSongForSceneFinal = 1;
			playSong(6);
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
		viewMatrix = bazierCameraForSceneFinal->getViewMatrix();
		camPos = bazierCameraForSceneFinal->getCameraPosition();
	}

	modelMatrixArray[0] = genrateModelMatrix(vec3(247.200897, 16.80, 249.101501f), vec3(0.000000, -128.000000, 0.000000), vec3(0.300000, 0.300000, 0.300000));
	modelPointLightStruct[0].pointLight_position = vec3(223.199829, 19.199, 220.79);
	modelPointLightStruct[1].pointLight_position = vec3(196.199829, 15.399, 200.199);
	modelPointLightStruct[2].pointLight_position = vec3(180.199829, 15.399, 186.199);
	modelPointLightStruct[3].pointLight_position = vec3(159.000015, 16.500002, 171.000015);
	modelPointLightStruct[4].pointLight_position = vec3(212.099731, 25.200003, 212.799728);
	modelPointLightStruct[5].pointLight_position = vec3(transformationVector.translationVector[0], transformationVector.translationVector[1], transformationVector.translationVector[2]);
	numOfPointLight = 6;
	objmodelLoadingShader->displayObjModelLoadingShader(&palaceInterior, modelMatrixArray, viewMatrix, 1, MODEL_POINTLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelDirectionLightStruct.directionLight_Direction = vec3(100, 50, 100);
	modelMatrixArray[0] = genrateModelMatrix(vec3(223.199600, 11.599997, 207.799), vec3(537.199524, -221.200165, 0.000), vec3(-3.10, -3.10, -3.10));
	objmodelLoadingShader->displayObjModelLoadingShader(&chanakyaStanding, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelDirectionLightStruct.directionLight_Direction = vec3(100, 50, 100);
	modelMatrixArray[0] = genrateModelMatrix(vec3(164.800201, 9.499997, 174.900), vec3(0.000000, 60.300037, 0.0), vec3(2.0));
	objmodelLoadingShader->displayObjModelLoadingShader(&dhananandKneel, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	//modelMatrix = genrateModelMatrix(vec3(transformationVector.translationVector[0], transformationVector.translationVector[1], transformationVector.translationVector[2]), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	//textureShader->useTextureShaderProgram();
	//textureShader->displayTextureShader(modelMatrix, viewMatrix, textures->getTexture("Stone"), 0);
	//renderes->renderSphere();
	//textureShader->unUseTextureShaderProgram();

	glBindTexture(GL_TEXTURE_2D, 0);
}
void Scene::updateSceneFinal() {
	if (!debugCamera)
	{
		if (bazierCameraForSceneFinal->time < 1.0f) {
			bazierCameraForSceneFinal->time += 0.00045f;
			bazierCameraForSceneFinal->update();
		}
	}
}
void Scene::unitializeSceneFinal() {

	atmosphericScatteringShader->deleteShaderProgramObject();
	if (atmosphericScatteringShader)
		delete atmosphericScatteringShader;

	objmodelLoadingShader->deleteShaderProgramObject();
	bazierCameraForSceneFinal->uninitialize();
}
