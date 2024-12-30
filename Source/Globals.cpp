#include"Globals.h"
#include "../Utility/Headers/Camera.h"


//File pointer
FILE* gpFile = NULL;
HWND gHwnd = NULL;
HDC gHdc = NULL;
HGLRC ghrc = NULL;

//Global active Window
BOOL bgActiveWindow = FALSE;
BOOL bFullScreen = FALSE;
const char gszLogFileName[] = "CustomLog.txt";


mat4 prespectiveProjectionMatrix;

//Scene number tracking varaiable
int currentSceneNumber = 0;
bool debugCamera = false;

float gWinWidth;
float gWinHeight;

//transformation vector
TransformationVector transformationVector;

std::vector<TransformationVector>transformationVectorArray; ///for storng  ttested dynamic loading of single model

Camera* freeCamera;

struct PointLight modelPointLightStruct[10];
int numOfPointLight;
struct DirectionLight modelDirectionLightStruct;

bool isDDSTextureClipped;



 mat4 genrateModelMatrix(vec3 translationValue, vec3 rotationValue, vec3 scaleValue) {

	 mat4 translation = mat4::identity();
	 mat4 rotation_X = mat4::identity();
	 mat4 rotation_Y = mat4::identity();
	 mat4 rotation_Z = mat4::identity();
	 mat4 rotation = mat4::identity();
	 mat4 scale = mat4::identity();
	 mat4 modelMatrix = mat4::identity();

	 translation = vmath::translate(translationValue[0], translationValue[1], translationValue[2]);

	 rotation_X = vmath::rotate(rotationValue[0], 1.0f, 0.0f, 0.0f);
	 rotation_Y = vmath::rotate(rotationValue[1], 0.0f, 1.0f, 0.0f);
	 rotation_Z = vmath::rotate(rotationValue[2], 0.0f, 0.0f, 1.0f);
	 rotation = rotation_X * rotation_Y * rotation_Z;

	 scale = vmath::scale(scaleValue[0], scaleValue[1], scaleValue[2]);

	 modelMatrix = translation * scale*rotation ;

	 return modelMatrix;
 }


 void FileLog(const char* fileName, int lineNumber, const char* formattedString, ...)
 {
	 //code
	 char buffer[1024];
	 char finalBuffer[1024];
	 DWORD bytesWritten = 0;

	 va_list args;
	 va_start(args, formattedString);
	 vsnprintf(buffer, 1024, formattedString, args);
	 va_end(args);

	 snprintf(finalBuffer, 1024, "[%s: (%d)] : %s", fileName, lineNumber, buffer);

	 HANDLE hFile = CreateFileA(
		 gszLogFileName,
		 FILE_APPEND_DATA,
		 FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		 NULL,
		 OPEN_ALWAYS,
		 FILE_ATTRIBUTE_NORMAL,
		 NULL);
	 if (INVALID_HANDLE_VALUE == hFile)
	 {
		 return;
	 }

	 WriteFile(hFile, finalBuffer, (DWORD)strlen(finalBuffer), &bytesWritten, NULL);

	 CloseHandle(hFile);

 }

