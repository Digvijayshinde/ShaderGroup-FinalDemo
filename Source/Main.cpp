//Header file
#include"Globals.h"
#include "../Scene/Headers/Scene.h"				//scene header file
#include "../Test/Headers/TestCommon.h"			//For testing purpose
#include "../Utility/Headers/CustomImGui.h"		//For ImGui
#include "../Utility/Headers/Camera.h"
#include "../Utility/Headers/AudioPlayer.h"


//Temp
#include "../Shaders/Ocean/OceanShader.h"


//Call back method
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global object creation Created only single instance of this object;
Test test;
Scene scene;

//FPS calculate 
LARGE_INTEGER startingTime, endingTime;
LARGE_INTEGER frequency;
double delta = 0.0;
double timeElapsed = 0.0;
short FPS_rate = 0;

LONGLONG tickspersec;
double last, current = 0.0;
double delta2, accum = 0;

//Imgui 
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int  WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow) {

	//function declaration
	int initialize(void);
	void display(float alpha, float elapsedtime);
	void update(void);

	void uninitialize(void);

	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");
	WNDCLASSEX wndclass;;

	BOOL bDone = FALSE;
	int iRetVal = 0;
	if (fopen_s(&gpFile, "Log.txt", "w") != 0) {
		MessageBox(NULL, TEXT("Log File Creation Failed"), TEXT("File I/O Error"), MB_OK);
		exit(0);
	}
	else {
		fprintf(gpFile, "Log file is open Successfully...\n");
	}


	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpszClassName = szAppName;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszMenuName = NULL;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, NULL);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hInstance = hInstance;
	wndclass.hIconSm = LoadIcon(hInstance,NULL);

	RegisterClassEx(&wndclass);

	//for getting the width and height of window
	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName,
		TEXT("Chanakya"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		x / 2 - WIN_WIDTH / 2,
		y / 2 - WIN_HEIGHT / 2,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	gHwnd = hwnd;
	//
	iRetVal = initialize();
	//uninitialize();
	if (iRetVal == -1) {
		fprintf(gpFile, "Choose Pixel Formate Failed\n");
		uninitialize();
	}
	else if (iRetVal == -2) {
		fprintf(gpFile, "Set Pixel Formate Failed\n");
		uninitialize();
	}
	else if (iRetVal == -3) {
		fprintf(gpFile, "Create OpenGL Contex Failed \n");
		uninitialize();;
	}
	else if (iRetVal == -4) {
		fprintf(gpFile, "Making OpenGl Context as current contex failed\n");
		uninitialize();
	}
	else if (iRetVal == -6) {
		fprintf(gpFile, "glew init failed\n");
		uninitialize();
	}
	else {
		fprintf(gpFile, "Initialization successfully\n");
	}
	ShowWindow(hwnd, iCmdShow);

	//forGrounding and focusing the window
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);


	QueryPerformanceFrequency(&frequency);
	tickspersec = frequency.QuadPart;

	QueryPerformanceCounter(&startingTime);
	last = (startingTime.QuadPart % tickspersec) / (double)tickspersec;
	float freq = 1.0f / (float)frequency.QuadPart;

	#pragma region initImgui
	#if DEBUG_MODE
		if (ImGuiInitialization() != 0) {
			fprintf(gpFile, "Failed in ImGuiInitialization\n");
			return -1;
		}
	#endif 
	#pragma endregion initImgui

	//Game Loop

	while (bDone == FALSE) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				bDone = TRUE;
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			if (bgActiveWindow == TRUE) {
				#pragma region ImGui
				#if DEBUG_MODE
								displayPrepareForImGui();
								displayImGui();
				#endif 
				#pragma endregion ImGui

				//game loop
								display((float)accum / 0.0333f, (float)delta2);

								// FPS Lock
								QueryPerformanceCounter(&endingTime);
								delta = (double)(endingTime.QuadPart - startingTime.QuadPart) * freq;
								current = (endingTime.QuadPart % tickspersec) / (double)tickspersec;

								if (current < last)
									delta2 = ((1.0 + current) - last);
								else
									delta2 = (current - last);

								last = current;
								accum += delta2;

				update();
			}
		}

	}
	uninitialize();
	return (msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {
	
	//Funtion declaration
	void TogglefullScreen();
	void resize(int, int);
	int currentModelNumber = 0;

	//Code
	
	#pragma region ImGuiWndProc
		#if DEBUG_MODE
			if (ImGui_ImplWin32_WndProcHandler(hwnd, imsg, wParam, lParam)) {
				return TRUE;
			}


		#endif
	#pragma endregion ImGuiWndProc	

	#pragma region WndProcScene
	#if SCENE_RENDER
				switch (currentSceneNumber) {
				case 1:
					scene.WndProcForSceneOne(hwnd, imsg, wParam, lParam);
					break;
				case 2:
					scene.WndProcForSceneTwo(hwnd, imsg, wParam, lParam);
					break;
				case 3:
					scene.WndProcForSceneThird(hwnd, imsg, wParam, lParam);
					break;
				}
	#endif

	#pragma endregion WndProcScene

	#pragma region WndProcTest
	#if TEST_RENDER
				test.WndProcForTest(hwnd, imsg, wParam, lParam);
	#endif
	#pragma endregion WndProcTest

	switch (imsg) {
	case WM_SETFOCUS:
		bgActiveWindow = TRUE;
		break;
	case WM_KILLFOCUS:
		bgActiveWindow = FALSE;
		break;
	case WM_ERASEBKGND:
		return 0;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOUSEMOVE:
	{
		float mouseX = LOWORD(lParam);
		float mouseY = HIWORD(lParam);

		// Send this to camera class
		freeCamera->mouseInputs(mouseX, mouseY);
	}
	break;

	case WM_MOUSEWHEEL:
	{
		short scrollDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		freeCamera->mouseScroll(scrollDelta);
	}
	break;
	case WM_CHAR:
		freeCamera->keyboardInputs(wParam);
		switch (wParam) {

		case 'F':
		case 'f':
			TogglefullScreen();
			break;
		case 27:
			PostQuitMessage(0);
			break;
		case 'n':
			currentSceneNumber++;
			break;
		if(ENABLE_MODEL_PLACING){
		case 'X':
			transformationVector.translationVector[0] += transformationVector.transformationSpeed;
			break;
		case 'x':
			transformationVector.translationVector[0] -= transformationVector.transformationSpeed;
			break;
		case 'Y':
			transformationVector.translationVector[1] += transformationVector.transformationSpeed;
			break;
		case 'y':
			transformationVector.translationVector[1] -= transformationVector.transformationSpeed;
			break;
		case 'Z':
			transformationVector.translationVector[2] += transformationVector.transformationSpeed;
			break;
		case 'z':
			transformationVector.translationVector[2] -= transformationVector.transformationSpeed;
			break;

			//rotate
		case 'J':
			transformationVector.rotationVector[0] += transformationVector.transformationSpeed;
			break;
		case 'j':
			transformationVector.rotationVector[0] -= transformationVector.transformationSpeed;
			break;
		case 'K':
			transformationVector.rotationVector[1] += transformationVector.transformationSpeed;
			break;
		case 'k':
			transformationVector.rotationVector[1] -= transformationVector.transformationSpeed;
			break;
		case 'L':
			transformationVector.rotationVector[2] += transformationVector.transformationSpeed;
			break;
		case 'l':
			transformationVector.rotationVector[2] -= transformationVector.transformationSpeed;
			break;

			//scale
		case '1':
			transformationVector.scaleVector[0] += transformationVector.transformationSpeed;
			break;
		case '2':
			transformationVector.scaleVector[0] -= transformationVector.transformationSpeed;
			break;
		case '3':
			//transformationVector.scaleVector[1] += transformationVector.transformationSpeed;
			transformationVector.scaleVector[1] += 0.05;

			break;
		case '4':
			//transformationVector.scaleVector[1] -= transformationVector.transformationSpeed;
			transformationVector.scaleVector[1] -= 0.05;

			break;
		case '5':
			transformationVector.scaleVector[2] += transformationVector.transformationSpeed;

			break;
		case '6':
			transformationVector.scaleVector[1] -= transformationVector.transformationSpeed;

			break;
		case '7':
			transformationVector.scaleVector[0] -= transformationVector.transformationSpeed;
			transformationVector.scaleVector[1] -= transformationVector.transformationSpeed;
			transformationVector.scaleVector[2]-= transformationVector.transformationSpeed;


			break;
		case '8':
			transformationVector.scaleVector[0] += transformationVector.transformationSpeed;
			transformationVector.scaleVector[1] += transformationVector.transformationSpeed;
			transformationVector.scaleVector[2] += transformationVector.transformationSpeed;
			break;
		case '9':
			transformationVector.transformationSpeed+=0.1;
			break;
		case '0':
			transformationVector.transformationSpeed -= 0.1;
			if (transformationVector.transformationSpeed < 0.0)
				transformationVector.transformationSpeed = 0.0;
				break;
		case 'r':
			transformationVector.resetTransformationVector();
			break;
		case 'p': 
			transformationVectorArray.push_back(transformationVector);
			for (auto itr = transformationVectorArray.begin(); itr != transformationVectorArray.end(); itr++) {
				PrintLog("\nTransformation for Model - %f\n Translation Vector = %f,%f,%f\nRotation Vector= %f,%f,%f\nScale Vector = %f,%f,%f\n----------------------------------------------------------\n",
					currentModelNumber++,
					(*itr).translationVector[0], (*itr).translationVector[1],
					(*itr).translationVector[2], (*itr).rotationVector[0], (*itr).rotationVector[1], (*itr).rotationVector[2],
					(*itr).scaleVector[0], (*itr).scaleVector[1], (*itr).scaleVector[2]);
			}
			PrintLog("Camera Position = vec3(%f,%f,%f)\n Camera Yaw = %f\n Camera Pitch = %f\n", freeCamera->position[0], freeCamera->position[1], freeCamera->position[2], freeCamera->yaw, freeCamera->pitch);
			break;
		case 'c':
			debugCamera = false;
			break;
		case 'C':
			debugCamera = true;
			break;
		/*case 'a':
			transformationVectorArray.push_back(transformationVector);
			break;*/
		}
		}
	break;
	default:
		break;
	}


	return(DefWindowProc(hwnd, imsg, wParam, lParam));

}
void TogglefullScreen() {

	MONITORINFO mi;
	static WINDOWPLACEMENT wp;
	static DWORD dwStyle;

	wp.length = sizeof(WINDOWPLACEMENT);
	if (bFullScreen == FALSE) {
		dwStyle = GetWindowLong(gHwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW) {
			mi.cbSize = sizeof(MONITORINFO);

			if (GetWindowPlacement(gHwnd, &wp) && GetMonitorInfo(MonitorFromWindow(gHwnd, MONITORINFOF_PRIMARY), &mi)) {
				SetWindowLong(gHwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(gHwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		#if DEBUG_MODE
			ShowCursor(TRUE);
		#else
			//ShowCursor(FALSE);
		#endif // DEBUG_MODE

		bFullScreen = TRUE;
	}
	else {
		SetWindowLong(gHwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(gHwnd, &wp);
		SetWindowPos(gHwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
		bFullScreen = FALSE;
	}


}
int  initializeAllScene() {
	if (scene.initialiseSceneFirst() != 0) {
		fprintf(gpFile, "failed\n");
	}
	if (scene.initialiseSceneSecond() != 0) {
		fprintf(gpFile, "failed\n");
	}
	if (scene.initialiseSceneOpening() != 0) {
		fprintf(gpFile, "failed\n");
	}
	if (scene.initialiseSceneEnd() != 0) {
		fprintf(gpFile, "failed\n");
	}
	return 0;
}
int initialize(void) {
	//function declaration
	void resize(int, int);
	void printGlInfo(void);
	void uninitialize(void);

	//variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	//code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));  //  or   memset((void*)&pfd,NULL,sizeof(PIXELFORMATEDESCRIPTOR));

	//initializing the pfd structure

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;  //24 alos can be done

	gHdc = GetDC(gHwnd);
	//ChoosePixelformate
	iPixelFormatIndex = ChoosePixelFormat(gHdc, &pfd);
	if (iPixelFormatIndex == 0) {
		return -1;
	}
	//SetChoosenPixelFormat
	if (SetPixelFormat(gHdc, iPixelFormatIndex, &pfd) == FALSE) {
		return -2;
	}

	//CreatOpenGlContex
	ghrc = wglCreateContext(gHdc);
	if (ghrc == NULL) {
		return -3;
	}

	//Make the rendering contex as current contex
	if (wglMakeCurrent(gHdc, ghrc) == FALSE) {
		return -4;
	}


	//GLEW initalization
	if (glewInit() != GLEW_OK) {
		return -5;
	}

	//wglewInit();
	//int attribs[] =
	//{
	//	WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
	//	WGL_CONTEXT_MINOR_VERSION_ARB, 6,
	//	WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
	//	WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
	//	0
	//};
	//HGLRC newrc = wglCreateContextAttribsARB(gHdc, NULL, attribs);
	//wglMakeCurrent(gHdc, newrc);

	freeCamera = new Camera(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, -10.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));


	#pragma region CommonRenderersInit
		//Object cration for renderes ....this object is accessiable in all files.
		if (renderes->initializeCubeRenderer() != 0) {
			fprintf(gpFile,"Failed to initialize cube\n");
			return -1;
		}
		if(renderes->initializePyramidRenderer()!=0){
			fprintf(gpFile, "Failed to initialize Pyramid\n");
			return -1;
		};
		if(renderes->initializeQuadRenderer()!=0){
			fprintf(gpFile, "Failed to initialize Quad\n");
			return -1;
		}
		if (renderes->initializeSphereRenderer() != 0) {
			fprintf(gpFile, "Failed to initialize sphere\n");
			return -1;
		}
	#pragma endregion CommonRenderersInit
#pragma region SceneRenderingInit
#if SCENE_RENDER
		switch (currentSceneNumber) {
		case 1:
			if (scene.initialiseSceneFirst() != 0) {
				fprintf(gpFile, "failed\n");
			}
			break;
		case 2:
			if (scene.initialiseSceneSecond() != 0) {
				fprintf(gpFile, "failed\n");
			}
			break;
		case 0:
			if (scene.initialiseSceneOpening() != 0) {
				fprintf(gpFile, "failed\n");
			}
			break;
		case 3:
			if (scene.initialiseSceneEnd() != 0) {
				fprintf(gpFile, "failed\n");
			}
			break;
		}
#endif

		//initialize all scene
#if ALL_SCENE_RENDER
		initializeAllScene();
#endif
	#pragma region TestingRenderingInit
		#if TEST_RENDER
			/*if (test.initializeTestColor() != 0) {
				fprintf(gpFile, "failed to initialize Test color\n");

			}*/
			if (test.initializeTestTexture() != 0) {
				fprintf(gpFile, "failed to initialize test texture\n");

			}
			//if (test.initializeTestFBORendering() != 0) {
			//	fprintf(gpFile, "failed to initialize test FBO rendering\n");
			//}
			if (test.initializeTestAtmosphereicScatteringendering() != 0) {
				fprintf(gpFile, "failed to initialize scattering  rendering\n");
			}
			//if (test.initializeTestCubeMap() != 0) {
			//	fprintf(gpFile, "failed to initialize cubemap\n");
			//}
			//if (test.initializeTestOcean() != 0) {
			//	fprintf(gpFile, "failed to initialize ocean\n");
			//}
			//if (test.initializeTestClothRendering() != 0) {
			//	fprintf(gpFile, "failed to initialize cloth\n");
			//}
			if (test.initializeTestTerrain() != 0) {
				fprintf(gpFile, "failed to initialize terrrain\n");
			}
			//if (test.initializeTestWaterRendering() != 0) {
			//	fprintf(gpFile, "failed to initialize Water\n");
			//}
				if (test.initializeTestObjModelLoading() != 0) {
				fprintf(gpFile, "failed to initialize model\n");
			}
		#endif 
	#pragma endregion TestingRenderingInit
			

	//deph and color related code
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	prespectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);

	glEnable(GL_TEXTURE_2D);

	////full screen now as start of application
	TogglefullScreen();

	//playSong(0);

	return 0;

}

void printGlInfo(void) {

	//local variable declartion
	GLint numExtensions = 0;

	//code
	fprintf(gpFile, "OpenGLVendor :    %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGLRenderer :  %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGLVersion :   %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "OpenGLSLVersion : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	//fprintf(gpFile, "OpenGLVersion :   %s\n", glGetString(GL_VERSION));

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	fprintf(gpFile, "Number of Suppported Extension %d\n", numExtensions);
	for (int i = 0; i < numExtensions; i++) {
		fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}
}
void resize(int width, int height) 
{

	if (height == 0)
		height = 1;

	glViewport(0, 0, GLsizei(width), GLsizei(height));
	gWinWidth = width;
	gWinHeight = height;
	prespectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 50000.0f);
}
void display(float alpha,float elapsedtime) {

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	#pragma region SceneRendering
	#if SCENE_RENDER || ALL_SCENE_RENDER:
		switch (currentSceneNumber) {
		case 0:
			scene.displaySceneOpening();
			break;
		case 1:
			scene.displaySceneFirst();
			break;
		case 2:
			scene.displaySceneSecond();
			break;
		case 3:
			scene.displaySceneEnd();
			break;
		}
	#endif
	#pragma endregion SceneRendering

	#pragma region TestRendering

		
		#if TEST_RENDER
			//test.displayTestColor();
			//test.displayTestVideoRendering();
			//test.displayTestFBORendering();
			//test.displayTestAtmosphereicScatteringRendering();
			//test.displayTestWaterRendering();
			//test.displayTestCubeMap();
			//test.displayTestOcean(alpha, elapsedtime);
			//test.displayTestClothRendering();
			//test.displayTestTerrain();
			//test.displayTestObjModelLoading();
			//test.displayTestTexture();

		#endif 
	#pragma endregion TestRendering		

	#pragma region ImGuiRendering
		#if DEBUG_MODE
				renderingImGui();
				
		#endif // DEBUG_MODE
	#pragma endregion ImGuiRendering
	SwapBuffers(gHdc);
}
void update(void) {

	//code
#if SCENE_RENDER ||ALL_SCENE_RENDER
	switch (currentSceneNumber) {
	case 0:
		scene.updateSceneOpening();
		break;
	case 1:
		scene.updateSceneFirst();
		break;
	case 2:
		scene.updateSceneSecond();
		break;
	case 3:
		scene.updateSceneEnd();
		break;
}

#endif

	#if TEST_RENDER
	//	test.updateTestVideoRendering();
	//test.updateTestOcean();
	//test.updateTestClothRendering();
	#endif 

}

void uninitialize(void) {

	//function declaration
	void TogglefullScreen();

	#pragma region uninitalize renderes

		renderes->unitializeCube();
		renderes->unitializePyramid();
		renderes->unitializeQuad();
		renderes->unitializeSphere();

		free(renderes);
		renderes = NULL;
	#pragma endregion uninitalize renderes

	#pragma region initialize scene
		scene.unitializeSceneFirst();
	#pragma endregion initialize scene

	#pragma region initialize Test

		test.uninitializeTestColor();
		test.uninitializeTestTexture();
		scene.unitializeSceneFirst();
		test.uninitializeTestFBORendering();
		test.uninitializeTestAtmosphereicScatteringRendering();
		test.uninitializeTestCubeMap();
		test.uninitializeTestOcean();
		//test.displayTestClothRendering();
	#pragma endregion initialize scene

	//to Avoid  exit window in full screen mode
	if (bFullScreen) {
		TogglefullScreen();
	}
	if (wglGetCurrentContext() == ghrc) {
		wglMakeCurrent(NULL, NULL);
	}
	if (ghrc) {
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}
	if (gHdc) {
		ReleaseDC(gHwnd, gHdc);
		gHdc = NULL;
	}
	if (gHwnd) {
		DestroyWindow(gHwnd);
		gHwnd = NULL;
	}
	if (gpFile) {
		fprintf(gpFile, "Log F	ile is Close Successfully...\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

