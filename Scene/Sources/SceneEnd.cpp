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

static AtmosphericScatteringShader* atmosphericScatteringShader = new AtmosphericScatteringShader();
extern StaticModel palace;
static StaticModel pikeman;
extern StaticModel chanakyaStanding;



static ObjModelLoadingShader* objmodelLoadingShader = new ObjModelLoadingShader();;

static int initCameraForSceneEnd= 0;
static int initSongForSceneEnd = 0;

static BezierCamera* bazierCameraForSceneEnd = new BezierCamera();

static TerrainShader* terrainShader = new TerrainShader();
static TextureManager* textures = new TextureManager();
static TextureManager* terrainTextures = new TextureManager();
static TextureShader* textureShader = new TextureShader();

static CubeMapShader* cubeMapShader = new CubeMapShader();
static unsigned int cubemapTexture;

static std::vector<std::string> facesCubeMap
{
	"Media\\Textures\\CubeMapNight\\left.jpg",
	"Media\\Textures\\CubeMapNight\\right.jpg",
	"Media\\Textures\\CubeMapNight\\up.jpg",
	"Media\\Textures\\CubeMapNight\\down.jpg",
	"Media\\Textures\\CubeMapNight\\front.jpg",
	"Media\\Textures\\CubeMapNight\\back.jpg"
};


void Scene::WndProcForSceneEnd(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {
	//code
}

int Scene::initialiseSceneEnd() {
	TextureManager textureManager;
	// Atmosphere Initialization
	if (atmosphericScatteringShader->initializeAtmosphericScatteringProgram() == -1)
		return -1;

	if (objmodelLoadingShader->initializeObjModelLoadingShaderProgram() != 0) {
		return -1;
	}
	initializeStaticModel(&palace, "Media/Models/CityWithCastle/city2.obj");
	initializeStaticModel(&pikeman, "Media/Models/pikeman/pike.obj");
	initializeStaticModel(&chanakyaStanding, "Media/Models/chanakya model/chanakya holding stick.obj");


	if (textureShader->initializeTextureShaderProgram() != 0) {
		return -1;
	}
	textures->storeTextureFromFile("Media\\Textures\\Terrain", "darksand.png");

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
	terrainTextures->storeTextureFromFile("Media\\Textures\\Terrain", "rock_normal.png");
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
	points.push_back({ 1245.495483,46.946224,339.782562 });
	points.push_back({ 570.993713,13.292456,127.925049 });
	points.push_back({ 1073.805786,20.773108,-75.034012 });
	points.push_back({ 1233.852173,20.773108,-290.075989 });
	points.push_back({ 914.476135,1.972801,-291.110229 });

	std::vector<float> yaw;
	yaw.push_back(-142.199585
	);
	yaw.push_back(42.80012);
	yaw.push_back(-97.999657);
	yaw.push_back(-180.1990);
	yaw.push_back(-182.598);


	std::vector<float> pitch;
	pitch.push_back(-7.400025
	);
	pitch.push_back(-0.800039

	);
	pitch.push_back(-4.200039
	);
	pitch.push_back(-1.800);
	pitch.push_back(16.199);

	bazierCameraForSceneEnd->initialize();
	bazierCameraForSceneEnd->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneEnd->time = 0;

	return 0;
}

void Scene::displaySceneEnd() {
	mat4 modelMatrixArray[500];

	if (initCameraForSceneEnd == 0) {
		initCameraForSceneEnd = 1;
		freeCamera->position = vec3(182.445862, 17.948416, 243.554276);
		freeCamera->front = vec3(0.999458, 0.027922, 0.017446);
		freeCamera->yaw = 1.000035;
		freeCamera->updateCameraVectors();
	}
	if (initSongForSceneEnd == 0) {
		if (AUDIO_ENABLE) {
			initSongForSceneEnd = 1;
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
		viewMatrix = bazierCameraForSceneEnd->getViewMatrix();
		camPos = bazierCameraForSceneEnd->getCameraPosition();
	}

	/*modelMatrix = genrateModelMatrix(vec3(261.800140, 0.0, 269.100159), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	atmosphericScatteringShader->useGroundAtmosphericScatteringProgram();
	atmosphericScatteringShader->displayGroundAtmosphericScatteringShader(modelMatrix, viewMatrix, camPos);
	atmosphericScatteringShader->unUseGroundAtmosphericScatteringProgram();
	atmosphericScatteringShader->useSkyAtmosphericScatteringProgram();
	atmosphericScatteringShader->displaySkyAtmosphericScatteringShader(modelMatrix, viewMatrix, camPos);
	atmosphericScatteringShader->unUseSkyAtmosphericScatteringProgram();*/

	modelDirectionLightStruct.directionLight_Direction = vec3(0,-10000, 0);
	modelMatrixArray[0] = genrateModelMatrix(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(5.900005, 5.900005, 5.90000));
	objmodelLoadingShader->displayObjModelLoadingShader(&palace, modelMatrixArray, viewMatrix,1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelDirectionLightStruct.directionLight_Direction = vec3(1500, 100.0, 0);
	modelMatrixArray[0] = genrateModelMatrix(vec3(874.499878, 0.000000, -326.999878),vec3(-179.999893, 67.499962, 0.000000), vec3(-8.499990, -8.499990, -8.499990));
	objmodelLoadingShader->displayObjModelLoadingShader(&chanakyaStanding, modelMatrixArray, viewMatrix, 1, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	for (unsigned int i = 0; i < 200; i++) {
			mat4 model = mat4(1.0f);

			// Positioning the soldiers
			float x = (i % 10) * 30.0f; // Columns
			float z = (i / 10) * 30.0f; // Rows
			//model = glm::translate(model, glm::vec3(x, 0.0f, z));

			modelMatrixArray[i] = genrateModelMatrix(vec3(894.420227+x, 0.000000, -248.884033+z), vec3(180,90.0,0.0), vec3(-10.079, -10.079, -10.07));
	}
	modelDirectionLightStruct.directionLight_Direction = vec3(1500, 100.0, 0);
	objmodelLoadingShader->displayObjModelLoadingShader(&pikeman, modelMatrixArray, viewMatrix, 200, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	for (unsigned int i = 0; i < 200; i++) {
		mat4 model = mat4(1.0f);

		// Positioning the soldiers
		float x = (i % 10) * 30.0f; // Columns
		float z = (i / 10) * 30.0f; // Rows
		//model = glm::translate(model, glm::vec3(x, 0.0f, z));

		modelMatrixArray[i] = genrateModelMatrix(vec3(894.420227 + x, 0.000000, -348.884033 - z), vec3(180, 90.0, 0.0), vec3(-10.079, -10.079, -10.07));
	}
	modelDirectionLightStruct.directionLight_Direction = vec3(1500, 100.0, 0);
	objmodelLoadingShader->displayObjModelLoadingShader(&pikeman, modelMatrixArray, viewMatrix, 200, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3(-4592.000000, -20.000004, 1464.000244), vec3(0.000000, -269.099945, 0.000000), vec3(1.0, 1.0, 1.0));
	terrainShader->useTerrainShaderProgram();
	terrainShader->lightPos = vec3(0.0, 0.0, 0.0);
	terrainShader->displayTerrainShader(terrainTextures, modelMatrix, viewMatrix, camPos, 536, 5.15);
	terrainShader->unUseTerrainShaderProgram();

	modelMatrix = genrateModelMatrix(vec3(0,0,0), vec3(0.0, 0.0, 0.0), vec3(20000.0, 20000.0, 20000.0));
	cubeMapShader->useCubeMapShaderProgram();
	cubeMapShader->displayCubeMapShader(cubemapTexture, modelMatrix, viewMatrix);
	renderes->renderCube();
	cubeMapShader->unUseCubeMapShaderProgram();

	modelMatrix = genrateModelMatrix(vec3(-50.0,-0.02,-50.0), vec3(-90.0,0.0,0.0), vec3(1500, 1500, 1500));
	textureShader->useTextureShaderProgram();
	textureShader->displayTextureShader(modelMatrix, viewMatrix, textures->getTexture("darksand"), 0);
	renderes->renderQuad();
	textureShader->unUseTextureShaderProgram();

	glBindTexture(GL_TEXTURE_2D, 0);
}
void Scene::updateSceneEnd() {
	if (!debugCamera)
	{
		if (bazierCameraForSceneEnd->time < 1.0f) {
			bazierCameraForSceneEnd->time += 0.00065f;
			bazierCameraForSceneEnd->update();
		}
	}
}
void Scene::unitializeSceneEnd() {

	atmosphericScatteringShader->deleteShaderProgramObject();
	if (atmosphericScatteringShader)
		delete atmosphericScatteringShader;

	objmodelLoadingShader->deleteShaderProgramObject();
	bazierCameraForSceneEnd->uninitialize();

	terrainShader->deleteShaderProgramObject();
	if (terrainShader) {
		delete terrainShader;
		terrainShader = NULL;
	}
}
