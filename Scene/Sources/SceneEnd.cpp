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

static ObjModelLoadingShader* objmodelLoadingShader = new ObjModelLoadingShader();;

static int initCameraForSceneEnd= 0;
static int initSongForSceneEnd = 0;

static BezierCamera* bazierCameraForSceneEnd = new BezierCamera();

static TerrainShader* terrainShader = new TerrainShader();
static TextureManager* textures = new TextureManager();
static TextureManager* terrainTextures = new TextureManager();


void Scene::WndProcForSceneEnd(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {
	//code
}

int Scene::initialiseSceneEnd() {
	// Atmosphere Initialization
	if (atmosphericScatteringShader->initializeAtmosphericScatteringProgram() == -1)
		return -1;

	if (objmodelLoadingShader->initializeObjModelLoadingShaderProgram() != 0) {
		return -1;
	}
	initializeStaticModel(&palace, "Media/Models/Arena/untitled.obj");
	textures->storeTextureFromFile("Media\\Textures\\Test", "Stone.bmp", ID_BITMAP_STONE);

	// Terrain initialization
	if (terrainShader->initializeTerrainShaderProgram("Media\\Textures\\Terrain\\height.png") != 0) {
		return -1;
	}
	else
	{
		fprintf(gpFile, "initialize initializeTerrainShaderProgram\n");
	}
	terrainTextures->storeTextureFromFile("Media\\Textures\\Terrain", "splat.png");
	terrainTextures->storeTextureFromFile("Media\\Textures\\Terrain", "grass.png");
	terrainTextures->storeTextureFromFile("Media\\Textures\\Terrain", "grass_normal.png");
	terrainTextures->storeTextureFromFile("Media\\Textures\\Terrain", "rock.png");
	terrainTextures->storeTextureFromFile("Media\\Textures\\Terrain", "rock_normal.png");
	terrainTextures->storeTextureFromFile("Media\\Textures\\Terrain", "NormalMap.png");

	//Bazier
	std::vector<std::vector<float>> points;
	points.push_back({ 210.140228,10013.521484,18.256680 });
	points.push_back({ 237.203827,10018.827148,166.284622 });
	points.push_back({ 232.422058,10026.605469,314.300812 });
	//points.push_back({ 141.431335,10014.075195,165.468811 });
	//points.push_back({ 139.737900,10016.585938,167.811462 });

	std::vector<float> yaw;
	yaw.push_back(56.40);
	yaw.push_back(42.80012);
	yaw.push_back(13.00020);
	//yaw.push_back(32.600090);
	//yaw.push_back(25.79);


	std::vector<float> pitch;
	pitch.push_back(-0.400006);
	pitch.push_back(3.199995
	);
	pitch.push_back(6.1999);
	//pitch.push_back(-0.400062);
	//pitch.push_back(-0.800019);

	bazierCameraForSceneEnd->initialize();
	bazierCameraForSceneEnd->setBezierPoints(points, yaw, pitch);
	bazierCameraForSceneEnd->time = 0;

	return 0;
}

void Scene::displaySceneEnd() {

	if (initCameraForSceneEnd == 0) {
		initCameraForSceneEnd = 1;
		freeCamera->position = vec3(182.445862, 10000 + 17.948416, 243.554276);
		freeCamera->front = vec3(0.999458, 0.027922, 0.017446);
		freeCamera->yaw = 1.000035;
		freeCamera->updateCameraVectors();
	}
	if (initSongForSceneEnd == 0) {
		if (AUDIO_ENABLE) {
			initSongForSceneEnd = 1;
			//playSong(0);
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

	modelMatrix = genrateModelMatrix(vec3(261.800140, 0.0, 269.100159), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	atmosphericScatteringShader->useGroundAtmosphericScatteringProgram();
	atmosphericScatteringShader->displayGroundAtmosphericScatteringShader(modelMatrix, viewMatrix, camPos);
	atmosphericScatteringShader->unUseGroundAtmosphericScatteringProgram();
	atmosphericScatteringShader->useSkyAtmosphericScatteringProgram();
	atmosphericScatteringShader->displaySkyAtmosphericScatteringShader(modelMatrix, viewMatrix, camPos);
	atmosphericScatteringShader->unUseSkyAtmosphericScatteringProgram();

	modelMatrix = genrateModelMatrix(vec3(458.000854, 10000 + 6.6, 323.4001), vec3(0.000000, -35.200005, 0.000000),vec3(5.900005, 5.900005, 5.90000));

	objmodelLoadingShader->displayObjModelLoadingShader(&palace, modelMatrix, viewMatrix, MODEL_DIRECTIONLIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	modelMatrix = genrateModelMatrix(vec3(-300.001526, 10000.0f, -300.101318), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	terrainShader->useTerrainShaderProgram();
	terrainShader->displayTerrainShader(terrainTextures, modelMatrix, viewMatrix, camPos,60,1.35);
	terrainShader->unUseTerrainShaderProgram();

	glBindTexture(GL_TEXTURE_2D, 0);
}
void Scene::updateSceneEnd() {
	if (!debugCamera)
	{
		if (bazierCameraForSceneEnd->time < 1.0f) {
			bazierCameraForSceneEnd->time += 0.00025f;
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
