#include"../../Source/Globals.h"
#include "../../Scene/Headers/Scene.h"
#include "../../Shaders/AtmosphericScattering/AtmosphericScattering.h"
#include "../../Shaders/Terrain/Terrain.h"
#include "../../Shaders/ObjModelLoading/ObjModelLoading.h"
#include "../../Utility/Headers/ModelLoading/StaticModelLoader.h"
#include "../../Shaders/CommomTexture/Texture.h"
#include "../../Utility/Headers/AudioPlayer.h"
#include "../../Shaders/WaterNew/Water.h"
#include "../../../Utility/Headers/BezierCamera.h"
#include"../../Shaders/Fire/Fire.h"
static FireShader* fireShader = new FireShader();

static AtmosphericScatteringShader* atmosphericScatteringShader = new AtmosphericScatteringShader();
static TerrainShader* terrainShader = new TerrainShader();
static TextureManager* terrainTextures = new TextureManager();

static StaticModel hut;
static StaticModel tree;
static StaticModel kund;
static StaticModel chanakyaSit;

static ObjModelLoadingShader* objmodelLoadingShader = new ObjModelLoadingShader();
static TextureShader* textureShader = new TextureShader();
static TextureManager* textures = new TextureManager();
static int initCameraForSceneSecond = 0;
static int initSongForSceneSecond = 0;

static WaterFrameBuffers* fbos;
static float moveFactor = 0.0f;
static bool isYFlipped = false;
static TextureManager* textureManager = new TextureManager();

static BezierCamera* bazierCameraForSceneSecond = new BezierCamera();
static BezierCamera* bazierCameraForSceneSecondTwo = new BezierCamera();

static int camNum = 0;



void captureWaterReflectionAndRefractionForSecondScene();

void Scene::WndProcForSceneTwo(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {
	//code
	switch (imsg)
	{
	case WM_CHAR:
		switch (wParam) {
		case 'p':
			PrintLog("AtmosPhere lightPos = vec3(%f,%f,%f) , mie constant = %f", atmosphericScatteringShader->lightPos[0], atmosphericScatteringShader->lightPos[1], atmosphericScatteringShader->lightPos[2] , atmosphericScatteringShader->mie_constant);
			break;
		case 'u':
			atmosphericScatteringShader->increaseLightPosition(0, 0.1);
			break;
		case 'i':
			atmosphericScatteringShader->increaseLightPosition(1, 0.1);
			break;
		case 'o':
			atmosphericScatteringShader->increaseLightPosition(2, 0.1);
			break;
		case 'U':
			atmosphericScatteringShader->decreaseLightPosition(0, 0.1);
			break;
		case 'I':
			atmosphericScatteringShader->decreaseLightPosition(1, 0.1);
			break;
		case 'O':
			atmosphericScatteringShader->decreaseLightPosition(2, 0.1);
		case 'm':
			atmosphericScatteringShader->increaseAndDecraseSunEmision(0, 0.001);
			break;
		case 'M':
			atmosphericScatteringShader->increaseAndDecraseSunEmision(1, 0.001);
			break;
			break;
		}
	}
}

int Scene::initialiseSceneSecond() {
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

	if (objmodelLoadingShader->initializeObjModelLoadingShaderProgram() != 0) {
		return -1;
	}
	initializeStaticModel(&hut, "Media/Models/Hut/newHut.obj");
	initializeStaticModel(&tree, "Media/Models/Hut/tree.obj");
	initializeStaticModel(&kund, "Media/Models/vedicFire/vedicFire.obj");
	initializeStaticModel(&chanakyaSit, "Media/Models/ChanakyaSitting/ChankyaSitting.obj");


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
	textures->storeTextureFromFile("Media\\Textures\\Test", "Stone.bmp", ID_BITMAP_STONE);

	transformationVector.translationVector = vec3(247.200897, 10000+31.29, 249.101501f);

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
	points.push_back({ 647.467285,10035.205078,391.743439 });
	points.push_back({ 309.008209,10038.392578,-43.265812 });
	points.push_back({ 7.075944,10041.414062,-6.258910 });
	//points.push_back({ -60.143433,10057.266602,157.501450 });

	std::vector<float> yaw;
	yaw.push_back(44.20085);
	yaw.push_back(66.400);
	yaw.push_back(61.001251);
	//yaw.push_back(37.401318);


	std::vector<float> pitch;
	pitch.push_back(4.9999);
	pitch.push_back(-0.000027);
	pitch.push_back(3.999969);
	//pitch.push_back(3.200027);

	/*Camera Position = vec3(382.108398, 10030.618164, 202.619995)
		Camera Yaw = 142.400162
		Camera Pitch = 5.999998*/

	bazierCameraForSceneSecond->initialize();
	bazierCameraForSceneSecond->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneSecond->time = 0;
	/////////////////////////////////////////////////////////////////////////
	points.clear();
	yaw.clear();
	pitch.clear();


	points.push_back({ 454.610107,10033.365234,182.469864 });
	points.push_back({ 143.963242,10038.930664,8.196793 });
	points.push_back({ -58.562290,10071.954102,185.159760 });
	//points.push_back({ -60.143433,10057.266602,157.501450 });


	yaw.push_back(143.600494);
	yaw.push_back(76.401085);
	yaw.push_back(34.001118);
	//yaw.push_back(37.401318);


	pitch.push_back(5.400011);
	pitch.push_back(0.400035);
	pitch.push_back(1.800052);

	bazierCameraForSceneSecondTwo->initialize();
	bazierCameraForSceneSecondTwo->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneSecondTwo->time = 0;

	///////////////////////////////////////////////////////////////////////////////////
	return 0;
}
void Scene::displaySceneSecond() {

	if (initCameraForSceneSecond == 0) {
		initCameraForSceneSecond = 1;
		freeCamera->position = vec3(182.445862,10000.0+17.948416, 243.554276);
		freeCamera->front = vec3(0.999458, 0.027922, 0.017446);
		freeCamera->yaw = 1.000035;
		freeCamera->updateCameraVectors();
	}
	if (initSongForSceneSecond == 0) {
		if (AUDIO_ENABLE) {
			initSongForSceneSecond = 1;
			playSong(1);
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
			viewMatrix = bazierCameraForSceneSecond->getViewMatrix();
			camPos = bazierCameraForSceneSecond->position;
		}
		if (camNum == 1)
		{
			viewMatrix = bazierCameraForSceneSecondTwo->getViewMatrix();
			camPos = bazierCameraForSceneSecondTwo->position;
		}
	}

	modelMatrix = genrateModelMatrix(vec3(-300.001526, 10000.0f, -300.101318), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	terrainShader->useTerrainShaderProgram();
	terrainShader->displayTerrainShader(terrainTextures, modelMatrix,viewMatrix, camPos,165, 1.35);
	terrainShader->unUseTerrainShaderProgram();

	modelMatrix = genrateModelMatrix(vec3(261.800140, 0.0, 269.100159), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	atmosphericScatteringShader->useGroundAtmosphericScatteringProgram();
	atmosphericScatteringShader->displayGroundAtmosphericScatteringShader(modelMatrix,viewMatrix, camPos);
	atmosphericScatteringShader->unUseGroundAtmosphericScatteringProgram();
	atmosphericScatteringShader->useSkyAtmosphericScatteringProgram();
	atmosphericScatteringShader->displaySkyAtmosphericScatteringShader(modelMatrix, viewMatrix, camPos);
	atmosphericScatteringShader->unUseSkyAtmosphericScatteringProgram();

	modelMatrix = genrateModelMatrix(vec3(-65.6998+144.600739, 10000+ 63.49, 390.899), vec3(-2.400000, -203.401306, 1.800), vec3(0.5, 0.5, 0.5));
	modelPointLightStruct[0].pointLight_position = vec3(3.000000, -0.900000, 8.100003);
	modelPointLightStruct[1].pointLight_position = transformationVector.translationVector;
	numOfPointLight = 2;

	modelDirectionLightStruct.directionLight_Direction = vec3(204.299652,10000+ 30.599987, 320.398865);
	objmodelLoadingShader->displayObjModelLoadingShader(&hut, modelMatrix, viewMatrix, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3( 144.600739, 10000 + 86.100113,390.899),vec3(0.0,0.0,0.0), vec3(1.0,1.0,1.0));
	modelPointLightStruct[0].pointLight_position = vec3(3.000000, -0.900000, 8.100003);
	modelPointLightStruct[1].pointLight_position = transformationVector.translationVector;
	numOfPointLight = 2;
	objmodelLoadingShader->displayObjModelLoadingShader(&tree, modelMatrix, viewMatrix, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3(-122.89 + 144.600739, 10000 + 49.499931, -22.399973 +390.899), vec3(-1.600000, -24.000004, 0.000000), vec3(8.200002, 8.200002, 8.200002));
	modelPointLightStruct[0].pointLight_position = vec3(3.000000, -0.900000, 8.100003);
	modelPointLightStruct[1].pointLight_position = transformationVector.translationVector;
	numOfPointLight = 2;
	objmodelLoadingShader->displayObjModelLoadingShader(&kund, modelMatrix, viewMatrix, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3(-108.900314 + 144.600739, 10000 + 52.899910, -15.699995 + 390.899),vec3(0.000000, -120.499992, 0.000000), vec3(7.600002, 7.600002, 7.600002));
	modelPointLightStruct[0].pointLight_position = vec3(3.000000, -0.900000, 8.100003);
	modelPointLightStruct[1].pointLight_position = transformationVector.translationVector;
	numOfPointLight = 2;
	objmodelLoadingShader->displayObjModelLoadingShader(&chanakyaSit, modelMatrix, viewMatrix, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3(21.500874, 10061.689453, 368.001343), vec3(9.299999, -9.299999, -176.700027), vec3(9.032018, 9.032018, 9.032018));
	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, textures->getTexture("alpha"), textures->getTexture("fire"), textures->getTexture("noise"));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	renderes->renderQuad();
	glDisable(GL_BLEND);

	captureWaterReflectionAndRefractionForSecondScene();
	modelMatrix = genrateModelMatrix(vec3(36.6 +313.600616, 10000+ 28.10, 47.69 +232.900238), vec3(0.0, 0.0, 0.0), vec3(710.0,0.0,710.0));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	fbos->draw_water(textureManager->getTexture("fftDudv"), textureManager->getTexture("fftNormal"), modelMatrix, viewMatrix, camPos);
	glDisable(GL_BLEND);

	/*modelMatrix = genrateModelMatrix(transformationVector.translationVector, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	textureShader->useTextureShaderProgram();
	textureShader->displayTextureShader(modelMatrix, viewMatrix, textures->getTexture("Stone"), 0);
	renderes->renderSphere();
	textureShader->unUseTextureShaderProgram();*/



	fireShader->unUseFireShaderProgram();
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Scene::updateSceneSecond() {
	fireShader->setFrameTime(0.001);
	if (!debugCamera)
	{
		if (bazierCameraForSceneSecond->time < 1.0f) {
			bazierCameraForSceneSecond->time += 0.0002f;
			bazierCameraForSceneSecond->update();
		}
		/*else
		{
			camNum = 1;
			if (bazierCameraForSceneSecondTwo->time < 1.0f) {
				bazierCameraForSceneSecondTwo->time += 0.0003f;
				bazierCameraForSceneSecondTwo->update();
			}
		}*/
	}
}
void Scene::unitializeSceneSecond() {

	atmosphericScatteringShader->deleteShaderProgramObject();
	if (atmosphericScatteringShader)
		delete atmosphericScatteringShader;

	terrainShader->deleteShaderProgramObject();
	if (terrainShader) {
		delete terrainShader;
		terrainShader = NULL;
	}

	objmodelLoadingShader->deleteShaderProgramObject();

	bazierCameraForSceneSecond->uninitialize();
}

void captureWaterReflectionAndRefractionForSecondScene()
{
	void invertPitch();
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	vec3 camPos;
	//capture reflection and refraction before framebuffer

	isYFlipped = true;

	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CLIP_DISTANCE0);

	resize(gWinWidth, gWinHeight);

	fbos->bindReflectionFrameBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (debugCamera)
	{
		freeCamera->invertPitch();
		viewMatrix = freeCamera->getViewMatrix();
		camPos = freeCamera->position;
	}
	else
	{
		if (camNum == 0)
		{
			bazierCameraForSceneSecond->invertPitch();
			viewMatrix = bazierCameraForSceneSecond->getViewMatrix();
			camPos = bazierCameraForSceneSecond->position;
		}
		if (camNum == 1)
		{
			bazierCameraForSceneSecondTwo->invertPitch();
			viewMatrix = bazierCameraForSceneSecondTwo->getViewMatrix();
			camPos = bazierCameraForSceneSecondTwo->position;
		}
	}

	// ------------------------------ Reflection------------------------------------------

	// Terrain
	modelMatrix = genrateModelMatrix(vec3(-300.001526, 10000.0f, -300.101318), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	terrainShader->useTerrainShaderProgram();
	terrainShader->displayTerrainShader(terrainTextures, modelMatrix, viewMatrix, camPos, 165, 1.35);
	terrainShader->unUseTerrainShaderProgram();

	// Atmosphere
	modelMatrix = genrateModelMatrix(vec3(261.800140, 0.0f, 269.100159), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	atmosphericScatteringShader->useGroundAtmosphericScatteringProgram();
	atmosphericScatteringShader->displayGroundAtmosphericScatteringShader(modelMatrix, viewMatrix, camPos);
	atmosphericScatteringShader->unUseGroundAtmosphericScatteringProgram();
	atmosphericScatteringShader->useSkyAtmosphericScatteringProgram();
	atmosphericScatteringShader->displaySkyAtmosphericScatteringShader(modelMatrix, viewMatrix, camPos);
	atmosphericScatteringShader->unUseSkyAtmosphericScatteringProgram();

	// Hut Model
	modelMatrix = genrateModelMatrix(vec3(-65.6998 + 144.600739, 10000 + 63.49, 230.00 + 160.899), vec3(-2.400000, -203.401306, 1.800), vec3(0.5, 0.5, 0.5));
	modelPointLightStruct[0].pointLight_position = vec3(3.000000, -0.900000, 8.100003);
	modelPointLightStruct[1].pointLight_position = transformationVector.translationVector;
	numOfPointLight = 2;
	objmodelLoadingShader->displayObjModelLoadingShader(&hut, modelMatrix, viewMatrix, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	//tree model
	modelMatrix = genrateModelMatrix(vec3(144.600739, 10000 + 86.100113, 390.899), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	modelPointLightStruct[0].pointLight_position = vec3(3.000000, -0.900000, 8.100003);
	modelPointLightStruct[1].pointLight_position = transformationVector.translationVector;
	numOfPointLight = 2;
	objmodelLoadingShader->displayObjModelLoadingShader(&tree, modelMatrix, viewMatrix, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3(-122.89 + 144.600739, 10000 + 49.499931, -22.399973 + 390.899), vec3(-1.600000, -24.000004, 0.000000), vec3(8.200002, 8.200002, 8.200002));
	modelPointLightStruct[0].pointLight_position = vec3(3.000000, -0.900000, 8.100003);
	modelPointLightStruct[1].pointLight_position = transformationVector.translationVector;
	numOfPointLight = 2;
	objmodelLoadingShader->displayObjModelLoadingShader(&kund, modelMatrix, viewMatrix, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3(-108.900314 + 144.600739, 10000 + 52.899910, -15.699995 + 390.899), vec3(0.000000, -120.499992, 0.000000), vec3(7.600002, 7.600002, 7.600002));
	modelPointLightStruct[0].pointLight_position = vec3(3.000000, -0.900000, 8.100003);
	modelPointLightStruct[1].pointLight_position = transformationVector.translationVector;
	numOfPointLight = 2;
	objmodelLoadingShader->displayObjModelLoadingShader(&chanakyaSit, modelMatrix, viewMatrix, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3(21.500874, 10061.689453, 368.001343), vec3(9.299999, -9.299999, -176.700027), vec3(9.032018, 9.032018, 9.032018));
	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, textures->getTexture("alpha"), textures->getTexture("fire"), textures->getTexture("noise"));

	renderes->renderQuad();

	// ------------------------------ Refraction------------------------------------------

	fbos->unbindCurrentFrameBuffer(gWinWidth, gWinHeight);

	if (debugCamera)
	{
		freeCamera->invertPitch();
		viewMatrix = freeCamera->getViewMatrix();
		camPos = freeCamera->position;
	}
	else
	{
		if (camNum == 0)
		{
			bazierCameraForSceneSecond->invertPitch();
			viewMatrix = bazierCameraForSceneSecond->getViewMatrix();
			camPos = bazierCameraForSceneSecond->position;
		}
		if (camNum == 1)
		{
			bazierCameraForSceneSecondTwo->invertPitch();
			viewMatrix = bazierCameraForSceneSecondTwo->getViewMatrix();
			camPos = bazierCameraForSceneSecondTwo->position;
		}
	}

	isYFlipped = false;
	fbos->bindRefractionFrameBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	modelMatrix = genrateModelMatrix(vec3(0.0, 0.00f, 0.0f), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	modelMatrix = genrateModelMatrix(vec3(-300.001526, 10000.0f, -300.101318), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	terrainShader->useTerrainShaderProgram();
	terrainShader->displayTerrainShader(terrainTextures, modelMatrix, viewMatrix, camPos, 165, 1.35);
	terrainShader->unUseTerrainShaderProgram();

	modelMatrix = genrateModelMatrix(vec3(461.800140, 0.0f, 469.100159), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	atmosphericScatteringShader->useGroundAtmosphericScatteringProgram();
	atmosphericScatteringShader->displayGroundAtmosphericScatteringShader(modelMatrix, viewMatrix, camPos);
	atmosphericScatteringShader->unUseGroundAtmosphericScatteringProgram();
	atmosphericScatteringShader->useSkyAtmosphericScatteringProgram();
	atmosphericScatteringShader->displaySkyAtmosphericScatteringShader(modelMatrix, viewMatrix, camPos);
	atmosphericScatteringShader->unUseSkyAtmosphericScatteringProgram();

	modelMatrix = genrateModelMatrix(vec3(-65.6998 + 144.600739, 10000 + 63.49, 230.00 + 160.899), vec3(-2.400000, -203.401306, 1.800), vec3(0.5, 0.5, 0.5));
	modelPointLightStruct[0].pointLight_position = vec3(3.000000, -0.900000, 8.100003);
	modelPointLightStruct[1].pointLight_position = transformationVector.translationVector;
	numOfPointLight = 2;
	objmodelLoadingShader->displayObjModelLoadingShader(&hut, modelMatrix, viewMatrix, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3(144.600739, 10000 + 86.100113, 390.899), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	modelPointLightStruct[0].pointLight_position = vec3(3.000000, -0.900000, 8.100003);
	modelPointLightStruct[1].pointLight_position = transformationVector.translationVector;
	numOfPointLight = 2;
	objmodelLoadingShader->displayObjModelLoadingShader(&tree, modelMatrix, viewMatrix, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3(-122.89 + 144.600739, 10000 + 49.499931, -22.399973 + 390.899), vec3(-1.600000, -24.000004, 0.000000), vec3(8.200002, 8.200002, 8.200002));
	modelPointLightStruct[0].pointLight_position = vec3(3.000000, -0.900000, 8.100003);
	modelPointLightStruct[1].pointLight_position = transformationVector.translationVector;
	numOfPointLight = 2;
	objmodelLoadingShader->displayObjModelLoadingShader(&kund, modelMatrix, viewMatrix, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3(-108.900314 + 144.600739, 10000 + 52.899910, -15.699995 + 390.899), vec3(0.000000, -120.499992, 0.000000), vec3(7.600002, 7.600002, 7.600002));
	modelPointLightStruct[0].pointLight_position = vec3(3.000000, -0.900000, 8.100003);
	modelPointLightStruct[1].pointLight_position = transformationVector.translationVector;
	numOfPointLight = 2;
	objmodelLoadingShader->displayObjModelLoadingShader(&chanakyaSit, modelMatrix, viewMatrix, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3(21.500874, 10061.689453, 368.001343), vec3(9.299999, -9.299999, -176.700027), vec3(9.032018, 9.032018, 9.032018));
	fireShader->useFireShaderProgram();
	fireShader->displayFireShader(modelMatrix, viewMatrix, textures->getTexture("alpha"), textures->getTexture("fire"), textures->getTexture("noise"));
	renderes->renderQuad();


	fbos->unbindCurrentFrameBuffer(gWinWidth, gWinHeight);

	glDisable(GL_CLIP_DISTANCE0);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);

}
//
//Translation Vector = 144.600739, 31.299984, 230.001556
//Rotation Vector = 0.000000, 0.000000, 0.000000
//Scale Vector = 49.899925, 1.000000, 1.000000