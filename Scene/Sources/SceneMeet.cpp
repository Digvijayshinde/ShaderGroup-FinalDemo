#include"../../Source/Globals.h"
#include "../../Scene/Headers/Scene.h"
#include "../../Shaders/ObjModelLoading/ObjModelLoading.h"
#include "../../Utility/Headers/ModelLoading/StaticModelLoader.h"
#include "../../Shaders/CommomTexture/Texture.h"
#include "../../Utility/Headers/AudioPlayer.h"
#include "../../../Utility/Headers/BezierCamera.h"
#include "../../Shaders/Fire/Fire.h"
#include "../../Shaders/Terrain/Terrain.h"
#include "../../Shaders/WaterNew/Water.h"
#include "../../Shaders/CubeMap/CubeMap.h"
#include "../../Shaders/WaterNew/Water.h"
#include "../../Shaders/GodRays/godRays.h"
#include "../../../Utility/Headers/FramebufferWithDepth.h"
#include"../../Utility/Headers/CustomImGui.h"

static WaterFrameBuffers* fbos = new WaterFrameBuffers();
static float moveFactor = 0.0f;
static bool isYFlipped = false;

static TextureShader* textureShader = new TextureShader();
static FireShader* fireShader = new FireShader();
static TextureManager* textureManager = new TextureManager();

static ObjModelLoadingShader* objmodelLoadingShader = new ObjModelLoadingShader();;

static int initCameraForSceneMeet = 0;
static int initSongForSceneMeet = 0;

static BezierCamera* bazierCameraForSceneMeet = new BezierCamera();
static BezierCamera* bazierCameraForSceneMeetTwo = new BezierCamera();

static TextureManager* textures = new TextureManager();
static TextureManager* terrainTextures = new TextureManager();

static StaticModel meet_scene;
extern StaticModel tree;
static StaticModel chandragupta;
extern StaticModel chanakyaStanding;

static TerrainShader* terrainShader = new TerrainShader();
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

static FrameBufferWithDepth* fbo_godRayspass1;
static FrameBufferWithDepth* fbo_godRayspass2;
static GodRaysShader* godRaysShader = new GodRaysShader();

void captureWaterReflectionAndRefractionForSceneMeet();


void Scene::WndProcForSceneMeet(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {
	//code
	switch (imsg)
	{
	case WM_CHAR:
		switch (wParam) {
		case 'p':
			PrintLog("Godrays lightPos = vec3(%f,%f,%f)\n", godRaysShader->lightPosition[0], godRaysShader->lightPosition[1], godRaysShader->lightPosition[2]);
			break;
		case 'u':
			godRaysShader->lightPosition[0] += 0.1;
			break;
		case 'i':
			godRaysShader->lightPosition[1] += 0.1;
			break;
		case 'o':
			godRaysShader->lightPosition[2] += 0.1;
			break;
		case 'U':
			godRaysShader->lightPosition[0] -= 0.1;
			break;
		case 'I':
			godRaysShader->lightPosition[1] -= 0.1;
			break;
		case 'O':
			godRaysShader->lightPosition[2] -= 0.1;
			break;
		case 'm':
			godRaysShader->exposure += 0.01;
			break;
		case 'M':
			godRaysShader->exposure -= 0.01;
			break;
		case 't':
			godRaysShader->decay += 0.01;
			break;
		case 'T':
			godRaysShader->decay -= 0.01;
			break;
		case 'h':
			godRaysShader->weight += 0.01;
			break;
		case 'H':
			godRaysShader->weight -= 0.01;
			break;
		}
	}
}

void Scene::ImGuiForSceneMeet() {

	ImGui::SliderFloat("exposure", &godRaysShader->exposure, 0.01, 100.0f);
	ImGui::SliderFloat("decay", &godRaysShader->decay, 0.01, 100.0f);
	ImGui::SliderFloat("weight", &godRaysShader->weight, 0.01, 100.0f);
	ImGui::SliderFloat("density", &godRaysShader->density, 0.01, 100.0f);

}

int Scene::initialiseSceneMeet() {

	fbo_godRayspass1 = new FrameBufferWithDepth(FBO_WIDTH, FBO_HEIGHT);
	fbo_godRayspass2 = new FrameBufferWithDepth(FBO_WIDTH, FBO_HEIGHT);

	if (godRaysShader->initializeGodRaysShaderProgram() != 0)
		return 1;

	// Atmosphere Initialization
	if (objmodelLoadingShader->initializeObjModelLoadingShaderProgram() != 0) {
		return -1;
	}
	initializeStaticModel(&meet_scene, "Media/Models/chanakya_chandra_Meet/meet_scene_model.obj");
	initializeStaticModel(&chandragupta, "Media/Models/chanakya_chandra_Meet/ChotaChandraModel.obj");

	//initializeStaticModel(&smallCity, "Media/Models/smallCity/base.obj");
	initializeStaticModel(&tree, "Media/Models/Hut/tree.obj");
	initializeStaticModel(&chanakyaStanding, "Media/Models/chanakya model/chanakya holding stick.obj");


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


	//Water initialization start
	fbos->initialization_water();
	textureManager->storeTextureFromFile("Media\\Textures\\Water", "waterDUDVmap.png");
	textureManager->storeTextureFromFile("Media\\Textures\\Water", "waterNormalMap.png");

	textureManager->storeTextureFromFile("Media\\Textures\\Water", "fftDudv.png");
	textureManager->storeTextureFromFile("Media\\Textures\\Water", "fftNormal.png");

	// Cubemap
	if (cubeMapShader->initializeCubeMapShaderProgram() != 0) {
		return -1;
	}
	cubemapTexture = textureManager->loadCubeMapTexture(facesCubeMap);

	// texture model
	if (textureShader->initializeTextureShaderProgram() != 0) {
		return -1;
	}
	textures->storeTextureFromFile("Media\\Textures\\Test", "Stone.bmp", ID_BITMAP_STONE);

	//Bazier
	std::vector<std::vector<float>> points;
	points.push_back({ -89.730087,10.315708,-194.981964 });
	points.push_back({ -86.367264,5.589392,-166.739471 });
	points.push_back({ -91.861496,-6.537002,-80.875023 });

	//points.push_back({ -88.388885,-0.461657,-117.278633 });
	points.push_back({ -102.166191,3.959286,-44.815582 });
	points.push_back({ -96.539246,7.288147,38.580273 });
	points.push_back({ -77.703049,5.528450,126.863922 });
	points.push_back({ -57.422092,4.727300,90.922874 });



	std::vector<float> yaw;
	yaw.push_back(78.200165);
	yaw.push_back(81.200577);
	yaw.push_back(78.000198);

	//yaw.push_back(87.000557);
	yaw.push_back(74.000717);
	yaw.push_back(48.800777);
	yaw.push_back(-48.999321);
	yaw.push_back(-2.199224);


	std::vector<float> pitch;
	pitch.push_back(-16.5999);
	pitch.push_back(-10.599989);
	pitch.push_back(-1.199997);

	//pitch.push_back(-0.199993);
	pitch.push_back(4.600010);
	pitch.push_back(-0.799990);
	pitch.push_back(0.200010);
	pitch.push_back(-1.399989);
	

	bazierCameraForSceneMeet->initialize();
	bazierCameraForSceneMeet->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneMeet->time = 0;

	points.clear();
	yaw.clear();
	pitch.clear();


	points.push_back({ -57.422092,4.727300,90.922874 });
	points.push_back({ -142.920547,22.487391,156.063812 });

	yaw.push_back(-2.199224);
	yaw.push_back(-49.000683
	);


	pitch.push_back(-1.399989);
	pitch.push_back(3.999964);

	bazierCameraForSceneMeetTwo->initialize();
	bazierCameraForSceneMeetTwo->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneMeetTwo->time = 0;

	return 0;
}

void meetScene(mat4 viewMatrix,vec3 camPos){
	mat4 modelMatrixArray[1];
	mat4 modelMatrix = mat4::identity();

	captureWaterReflectionAndRefractionForSceneMeet();


	fbo_godRayspass1->start();
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//////////////////////////////////
		objmodelLoadingShader->enableGodRays = 1;
		objmodelLoadingShader->isColor_godRays = 0;
		modelDirectionLightStruct.directionLight_Direction = vec3(-50.600739, 50.00, 100.899);
		modelMatrixArray[0] = genrateModelMatrix(vec3(-52.199917, -1.200000, 94.50), vec3(0.600000, -171.000977, 0.00), vec3(12.700005, 12.700005, 12.700005));
		//modelMatrixArray[0] = genrateModelMatrix(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(5.900005, 5.900005, 5.90000));
		objmodelLoadingShader->displayObjModelLoadingShader(&meet_scene, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
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

		if (camNum == 0)
		{
			modelDirectionLightStruct.directionLight_Direction = vec3(0.600739, 50.00, 0.899);
			//modelMatrixArray[0] = genrateModelMatrix(transformationVector.translationVector, transformationVector.rotationVector, transformationVector.scaleVector);
			modelMatrixArray[0] = genrateModelMatrix(vec3(-59.399899, -0.900000, -21.899994), vec3(0.0, 0.0, 0.0), vec3(5.5, 5.5, 5.5));
			objmodelLoadingShader->displayObjModelLoadingShader(&chanakyaStanding, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		if (camNum == 1)
		{
			modelDirectionLightStruct.directionLight_Direction = vec3(0.600739, 50.00, 0.899);
			//modelMatrixArray[0] = genrateModelMatrix(transformationVector.translationVector, transformationVector.rotationVector, transformationVector.scaleVector);
			modelMatrixArray[0] = genrateModelMatrix(vec3(-57.000031, -2.400000, 112.200241), vec3(0.000000, -147.299881, 0.000000), vec3(4.5, 4.5, 4.5));
			objmodelLoadingShader->displayObjModelLoadingShader(&chanakyaStanding, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		modelMatrix = genrateModelMatrix(vec3(266.60,145.50,-202.20), vec3(0.0, 0.0, 0.0), vec3(58.60,58.60,58.60));
		textureShader->useTextureShaderProgram();
		textureShader->enableGodRays =1;
		textureShader->isColor_godRays =1;
		textureShader->displayTextureShader(modelMatrix, viewMatrix, textures->getTexture("Stone"), 0);
		renderes->renderSphere();
		textureShader->unUseTextureShaderProgram();
		glBindTexture(GL_TEXTURE_2D, 0);
		///////////////////////////////////////////////////////////////////
	}
	fbo_godRayspass1->end();


	fbo_godRayspass2->start();
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//////////////////////////////////

		objmodelLoadingShader->enableGodRays = 0;
		objmodelLoadingShader->isColor_godRays = 0;

		modelDirectionLightStruct.directionLight_Direction = vec3(-50.600739, 50.00, 100.899);
		modelMatrixArray[0] = genrateModelMatrix(vec3(-52.199917, -1.200000, 94.50), vec3(0.600000, -171.000977, 0.00), vec3(12.700005, 12.700005, 12.700005));
		//modelMatrixArray[0] = genrateModelMatrix(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(5.900005, 5.900005, 5.90000));
		objmodelLoadingShader->displayObjModelLoadingShader(&meet_scene, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
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

		if (camNum == 0)
		{
			modelDirectionLightStruct.directionLight_Direction = vec3(0.600739, 50.00, 0.899);
			//modelMatrixArray[0] = genrateModelMatrix(transformationVector.translationVector, transformationVector.rotationVector, transformationVector.scaleVector);
			modelMatrixArray[0] = genrateModelMatrix(vec3(-59.399899, -0.900000, -21.899994), vec3(0.0, 0.0, 0.0), vec3(5.5, 5.5, 5.5));
			objmodelLoadingShader->displayObjModelLoadingShader(&chanakyaStanding, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		if (camNum == 1)
		{
			modelDirectionLightStruct.directionLight_Direction = vec3(0.600739, 50.00, 0.899);
			//modelMatrixArray[0] = genrateModelMatrix(transformationVector.translationVector, transformationVector.rotationVector, transformationVector.scaleVector);
			modelMatrixArray[0] = genrateModelMatrix(vec3(-57.000031, -2.400000, 112.200241), vec3(0.000000, -147.299881, 0.000000), vec3(4.5, 4.5, 4.5));
			objmodelLoadingShader->displayObjModelLoadingShader(&chanakyaStanding, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		terrainShader->useTerrainShaderProgram();
		terrainShader->lightPos = vec3(0.0, 0.0, 0.0);
		modelMatrix = genrateModelMatrix(vec3(-151.499908, -2.700000, -404.098), vec3(0.000000, 0.0, 0.00), vec3(1.0, 1.0, 1.0));
		terrainShader->displayTerrainShader(terrainTextures, modelMatrix, viewMatrix, camPos, 46.900017, 0.60);
		terrainShader->unUseTerrainShaderProgram();

		terrainShader->useTerrainShaderProgram();
		terrainShader->lightPos = vec3(0.0, 0.0, 0.0);
		modelMatrix = genrateModelMatrix(vec3(319.600006, -2.599983, 597.000732), vec3(0.000000, -171.000061, 0.000000), vec3(1.0));
		terrainShader->displayTerrainShader(terrainTextures, modelMatrix, viewMatrix, camPos, 41.699921, 0.60000);
		terrainShader->unUseTerrainShaderProgram();

		modelMatrix = genrateModelMatrix(vec3(0.0), vec3(0.0, -200.0, 0.0), vec3(500.0, 500.0, 500.0));
		cubeMapShader->useCubeMapShaderProgram();
		cubeMapShader->displayCubeMapShader(cubemapTexture, modelMatrix, viewMatrix);
		renderes->renderCube();
		cubeMapShader->unUseCubeMapShaderProgram();
		glBindTexture(GL_TEXTURE_2D, 0);

		modelMatrix = genrateModelMatrix(vec3(19.801205, -1.000008, -181.699982), vec3(0.0), vec3(167.499893, 167.499893, 167.499893));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		fbos->draw_water(textureManager->getTexture("waterDUDVmap"), textureManager->getTexture("waterNormalMap"), modelMatrix, viewMatrix, camPos);
		glDisable(GL_BLEND);

		//modelMatrix = genrateModelMatrix(vec3(transformationVector.translationVector), vec3(0.0, 0.0, 0.0), transformationVector.scaleVector);
		//textureShader->useTextureShaderProgram();
		//textureShader->displayTextureShader(modelMatrix, viewMatrix, textures->getTexture("Stone"), 0);
		//renderes->renderSphere();
		//textureShader->unUseTextureShaderProgram();
		//glBindTexture(GL_TEXTURE_2D, 0);

		///////////////////////////////////////////////////////////////////
	}
	fbo_godRayspass2->end();


	glClearColor(0.3, 0.3, 0.3, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, gWinWidth, gWinHeight);

	//godRaysShader->lightPosition = vec4(10,10,10,1.0);
	godRaysShader->useGodRaysShaderProgram();
	godRaysShader->displayGodRaysShader(modelMatrix,viewMatrix,fbo_godRayspass1->colorTexture, fbo_godRayspass2->colorTexture);
	renderes->renderQuad();
	godRaysShader->unUseGodRaysShaderProgram();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Scene::displaySceneMeet() {
	mat4 modelMatrixArray[1];
	mat4 modelMatrix = mat4::identity();

	if (initCameraForSceneMeet == 0) {
		initCameraForSceneMeet = 1;
		freeCamera->position = vec3(0.0, 0.0, 0.0);
		freeCamera->front = vec3(0.999458, 0.027922, 0.017446);
		freeCamera->yaw = 1.000035;
		freeCamera->updateCameraVectors();
	}
	if (initSongForSceneMeet == 0) {
		if (AUDIO_ENABLE) {
			initSongForSceneMeet = 1;
			playSong(3);
		}
	}

	mat4 viewMatrix = mat4::identity();
	vec3 camPos;
	if (debugCamera)
	{
		viewMatrix = freeCamera->getViewMatrix();
		camPos = freeCamera->position;

		meetScene(viewMatrix, camPos);
	}
	else
	{
		if (camNum == 0)
		{
			viewMatrix = bazierCameraForSceneMeet->getViewMatrix();
			camPos = bazierCameraForSceneMeet->getCameraPosition();

			meetScene(viewMatrix, camPos);

		}
		if (camNum == 1)
		{
			viewMatrix = bazierCameraForSceneMeetTwo->getViewMatrix();
			camPos = bazierCameraForSceneMeetTwo->position;

			meetScene(viewMatrix, camPos);
		}
	}
	
}

void Scene::updateSceneMeet() {
	if (!debugCamera)
	{
		if (bazierCameraForSceneMeet->time < 1.0f) {
			bazierCameraForSceneMeet->time += 0.00055f;
			bazierCameraForSceneMeet->update();
		}
		else
		{
			camNum = 1;
			if (bazierCameraForSceneMeetTwo->time < 1.0f) {
				bazierCameraForSceneMeetTwo->time += 0.0005f;
				bazierCameraForSceneMeetTwo->update();
			}
		}

		if (cameraReset)
		{
			bazierCameraForSceneMeet->time = 0.0f;
			bazierCameraForSceneMeetTwo->time = 0.0f;
			cameraReset = false;
		}
	}


	fireShader->setFrameTime(0.001);
}

void Scene::unitializeSceneMeet() {

	objmodelLoadingShader->deleteShaderProgramObject();
	bazierCameraForSceneMeet->uninitialize();
}

void captureWaterReflectionAndRefractionForSceneMeet()
{
	mat4 modelMatrixArray[1];
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
			bazierCameraForSceneMeet->invertPitch();
			viewMatrix = bazierCameraForSceneMeet->getViewMatrix();
			camPos = bazierCameraForSceneMeet->getCameraPosition();
		}
		if (camNum == 1)
		{
			bazierCameraForSceneMeetTwo->invertPitch();
			viewMatrix = bazierCameraForSceneMeetTwo->getViewMatrix();
			camPos = bazierCameraForSceneMeetTwo->position;
		}
	}

	modelDirectionLightStruct.directionLight_Direction = vec3(-50.600739, 50.00, 100.899);
	modelMatrixArray[0] = genrateModelMatrix(vec3(-52.199917, -1.200000, 94.50), vec3(0.600000, -171.000977, 0.00), vec3(12.700005, 12.700005, 12.700005));
	//modelMatrixArray[0] = genrateModelMatrix(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(5.900005, 5.900005, 5.90000));
	objmodelLoadingShader->displayObjModelLoadingShader(&meet_scene, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);


	modelDirectionLightStruct.directionLight_Direction = vec3(-50.600739, 50.00, 100.899);
	modelMatrixArray[0] = genrateModelMatrix(vec3(-42.700001, 14.000002, 86.700), vec3(0.600000, 0.0, 0.00), vec3(0.400, 0.400, 0.400));
	//modelMatrixArray[0] = genrateModelMatrix(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(5.900005, 5.900005, 5.90000));
	objmodelLoadingShader->displayObjModelLoadingShader(&tree, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelDirectionLightStruct.directionLight_Direction = vec3(0.600739, 50.00, 0.899);
	//modelMatrixArray[0] = genrateModelMatrix(transformationVector.translationVector, transformationVector.rotationVector, transformationVector.scaleVector);
	modelMatrixArray[0] = genrateModelMatrix(vec3(-59.399899, -0.900000, -21.899994), vec3(0.0, 0.0, 0.0), vec3(5.5, 5.5, 5.5));
	objmodelLoadingShader->displayObjModelLoadingShader(&chanakyaStanding, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	terrainShader->useTerrainShaderProgram();
	terrainShader->lightPos = vec3(0.0, 0.0, 0.0);
	modelMatrix = genrateModelMatrix(vec3(-151.499908, -2.700000, -404.098), vec3(0.000000, 0.0, 0.00), vec3(1.0, 1.0, 1.0));
	terrainShader->displayTerrainShader(terrainTextures, modelMatrix, viewMatrix, camPos, 46.900017, 0.60);
	terrainShader->unUseTerrainShaderProgram();


	modelMatrix = genrateModelMatrix(transformationVector.translationVector, vec3(0.0, -200.0, 0.0), vec3(500.0, 500.0, 500.0));
	cubeMapShader->useCubeMapShaderProgram();
	cubeMapShader->displayCubeMapShader(cubemapTexture, modelMatrix, viewMatrix);
	renderes->renderCube();
	cubeMapShader->unUseCubeMapShaderProgram();
	glBindTexture(GL_TEXTURE_2D, 0);


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
			bazierCameraForSceneMeet->invertPitch();
			viewMatrix = bazierCameraForSceneMeet->getViewMatrix();
			camPos = bazierCameraForSceneMeet->getCameraPosition();
		}
		if (camNum == 1)
		{
			bazierCameraForSceneMeetTwo->invertPitch();
			viewMatrix = bazierCameraForSceneMeetTwo->getViewMatrix();
			camPos = bazierCameraForSceneMeetTwo->position;
		}
	}

	isYFlipped = false;

	// ------------------------------ Refraction------------------------------------------
	fbos->bindRefractionFrameBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	modelDirectionLightStruct.directionLight_Direction = vec3(-50.600739, 50.00, 100.899);
	modelMatrixArray[0] = genrateModelMatrix(vec3(-52.199917, -1.200000, 94.50), vec3(0.600000, -171.000977, 0.00), vec3(12.700005, 12.700005, 12.700005));
	//modelMatrixArray[0] = genrateModelMatrix(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(5.900005, 5.900005, 5.90000));
	objmodelLoadingShader->displayObjModelLoadingShader(&meet_scene, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelDirectionLightStruct.directionLight_Direction = vec3(-50.600739, 50.00, 100.899);
	modelMatrixArray[0] = genrateModelMatrix(vec3(-42.700001, 14.000002, 86.700), vec3(0.600000, 0.0, 0.00), vec3(0.400, 0.400, 0.400));
	//modelMatrixArray[0] = genrateModelMatrix(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(5.900005, 5.900005, 5.90000));
	objmodelLoadingShader->displayObjModelLoadingShader(&tree, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelDirectionLightStruct.directionLight_Direction = vec3(0.600739, 50.00, 0.899);
	//modelMatrixArray[0] = genrateModelMatrix(transformationVector.translationVector, transformationVector.rotationVector, transformationVector.scaleVector);
	modelMatrixArray[0] = genrateModelMatrix(vec3(-59.399899, -0.900000, -21.899994), vec3(0.0, 0.0, 0.0), vec3(5.5, 5.5, 5.5));
	objmodelLoadingShader->displayObjModelLoadingShader(&chanakyaStanding, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	terrainShader->useTerrainShaderProgram();
	terrainShader->lightPos = vec3(0.0, 0.0, 0.0);
	modelMatrix = genrateModelMatrix(vec3(-151.499908, -2.700000, -404.098), vec3(0.000000, 0.0, 0.00), vec3(1.0, 1.0, 1.0));
	terrainShader->displayTerrainShader(terrainTextures, modelMatrix, viewMatrix, camPos, 46.900017, 0.60);
	terrainShader->unUseTerrainShaderProgram();

	modelMatrix = genrateModelMatrix(transformationVector.translationVector, vec3(0.0, -200.0, 0.0), vec3(500.0, 500.0, 500.0));
	cubeMapShader->useCubeMapShaderProgram();
	cubeMapShader->displayCubeMapShader(cubemapTexture, modelMatrix, viewMatrix);
	renderes->renderCube();
	cubeMapShader->unUseCubeMapShaderProgram();
	glBindTexture(GL_TEXTURE_2D, 0);

	fbos->unbindCurrentFrameBuffer(gWinWidth, gWinHeight);

	glDisable(GL_CLIP_DISTANCE0);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);

}
