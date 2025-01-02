#pragma once	

#include"Common.h"
#include "BMP_Header.h"											//For the BMP textures.
#include "../Utility/Headers/Camera.h"

//File pointer declaration
extern FILE* gpFile;
extern HWND gHwnd;
extern HDC gHdc;
extern HGLRC ghrc;

//Global active Window
extern BOOL bgActiveWindow;
extern BOOL bFullScreen;

extern mat4 prespectiveProjectionMatrix;

//renderes object creation this object will use through out 
static Renderers* renderes = new Renderers();

//Scene number tracking varaiable
extern int currentSceneNumber;
extern bool isDDSTextureClipped;

//Shader enum
enum
{
	SHADER_ATTRIBUTE_POSITION = 0,
	SHADER_ATTRIBUTE_COLOR,
	SHADER_ATTRIBUTE_NORMAL,
	SHADER_ATTRIBUTE_TEXTURE0,
	SHADER_ATTRIBUTE_TANGENT,
	SHADER_ATTRIBUTE_BITANGENT,
	SHADER_ATTRIBUTE_BONEIDS,
	SHADER_ATTRIBUTE_WEIGHT,


};

//transformation vector
struct TransformationVector {
public:
	vec3 translationVector;
	vec3 rotationVector;
	vec3 scaleVector;
	float transformationSpeed;

	TransformationVector() {
		translationVector = vec3(0.0f, 0.0f, 0.0f);
		rotationVector = vec3(0.0f, 0.0f, 0.0f);
		scaleVector = vec3(1.0f, 1.0f, 1.0f);
		transformationSpeed = 0.3f;
	}
	void resetTransformationVector() {
		translationVector = vec3(0.0f, 0.0f, 0.0f);
		rotationVector = vec3(0.0f, 0.0f, 0.0f);
		scaleVector = vec3(1.0f, 1.0f, 1.0f);
		transformationSpeed = 0.05f;
	}
};
//Macro defination
//In Debug mode imgui is actvated

#define ENABLE_MODEL_PLACING 0
#define SCENE_RENDER 1
#define TEST_RENDER 0
#define DEBUG_MODE 0
#define AUDIO_ENABLE 1
#define ALL_SCENE_RENDER 0

//Scene number tracking varaiable
extern int currentSceneNumber;
extern bool debugCamera;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define MAX_ATTACHMENTS 4
#define FBO_WIDTH 2560
#define FBO_HEIGHT 1440

extern TransformationVector transformationVector;
extern std::vector<TransformationVector>transformationVectorArray; 

extern float gWinWidth;
extern float gWinHeight;
mat4 genrateModelMatrix(vec3 translationValue, vec3 rotationValue, vec3 scaleValue);

//FBO related variables
typedef struct _FrameBuffer
{
	GLuint fbo;
	GLuint rbo;
	GLuint fboTexture[MAX_ATTACHMENTS];
} FrameBuffer;


enum MODEL_EFFECT {

	MODEL_NO_EFFECT = 0,
	MODEL_POINTLIGHT,
	MODEL_DIRECTIONLIGHT,
};

struct PointLight {
	vec3 pointLight_position;
	vec3 pointLight_ambient = vec3(0.1f, 0.1f, 0.1f);
	vec3 pointLight_diffuse = vec3(1.0f, 1.0f, 1.0f);
	vec3 pointLight_specular = vec3(1.0f, 1.0f, 1.0f);;
	float pointLight_constant = 1.0f;
	float pointLight_linear = 0.09f;
	float pointLight_quadratic = 0.03f;

	void resetDefaultColor() {
		pointLight_position = vec3(0.0f, 0.0f, -10.0f);
		pointLight_ambient = vec3(0.1f, 0.1f, 0.1f);
		pointLight_diffuse = vec3(1.0f, 1.0f, 1.0f);
		pointLight_specular = vec3(1.0f, 1.0f, 1.0f);;
		pointLight_constant = 0.2f;
		pointLight_linear = 0.09f;
		pointLight_quadratic = 0.03f;
	}
};
extern struct PointLight modelPointLightStruct[20];
extern  int numOfPointLight;

struct DirectionLight {
	vec3 directionLight_Direction = vec3(0.0f, 10.0f, 50.0f);
	vec3 directionLight_ambient = vec3(0.1f, 0.1f, 0.1f);
	vec3 directionLight_diffuse = vec3(1.0f, 1.0f, 1.0f);
	vec3 directionLight_specular = vec3(1.0f, 1.0f, 1.0f);
	float directionLight_shininess = 128.0f;
};
extern struct DirectionLight modelDirectionLightStruct;


//log function
void FileLog(const char* fileName, int lineNumber, const char* formattedString, ...);

#define PrintLog(formattedString, ...) \
	FileLog(__FILE__, __LINE__, formattedString, __VA_ARGS__)


//Ocean related 
enum OpenGLDeclType
{
	GLDECLTYPE_FLOAT1 = 0,
	GLDECLTYPE_FLOAT2 = 1,
	GLDECLTYPE_FLOAT3 = 2,
	GLDECLTYPE_FLOAT4 = 3,
	GLDECLTYPE_GLCOLOR = 4
};

enum OpenGLDeclUsage
{
	GLDECLUSAGE_POSITION = 0,
	GLDECLUSAGE_BLENDWEIGHT,
	GLDECLUSAGE_BLENDINDICES,
	GLDECLUSAGE_NORMAL,
	GLDECLUSAGE_PSIZE,
	GLDECLUSAGE_TEXCOORD,
	GLDECLUSAGE_TANGENT = GLDECLUSAGE_TEXCOORD + 8,
	GLDECLUSAGE_BINORMAL,
	GLDECLUSAGE_TESSFACTOR,
	GLDECLUSAGE_POSITIONT,
	GLDECLUSAGE_COLOR,
	GLDECLUSAGE_FOG,
	GLDECLUSAGE_DEPTH,
	GLDECLUSAGE_SAMPLE
};
enum OpenGLLockFlags
{
	GLLOCK_READONLY = GL_MAP_READ_BIT,
	GLLOCK_DISCARD = GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT
};


#define GL_SAFE_DELETE_BUFFER(x) \
	if (x != 0) { \
		glDeleteBuffers(1, &x); \
		x = 0; }

extern Camera* freeCamera;
extern bool cameraReset;

void resize(int width, int height);

//FPS macros
#define ENABLE_FPS_LOCK 1
#define DESIRED_FPS 60

enum {
	NO_FADEIN_FADEOUT,
	FADE_IN,
	FADE_OUT,
};

void setBlendingMode(int mode, float fadeinFadeOutSpeed = 0.002f);