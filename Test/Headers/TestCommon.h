#pragma once

//All testing related  function is presentin this file. this is commmon header for all testing effect ....

//Header files  
#include "../../Source/Globals.h"

class Test {

public:
	//constructor
	Test(){}

	//This wndProc is present in onefile  at a time to handel key event
	void WndProcForTest(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam);

	//method
	int initializeTestColor();
	void displayTestColor();
	void updateTestColor();
	void uninitializeTestColor();

	//texture
	int initializeTestTexture();
	void displayTestTexture();
	void updateTestTexture();
	void uninitializeTestTexture();

	//Video rendring
	int initializeTestVideoRendering();
	void displayTestVideoRendering();
	void updateTestVideoRendering();
	void uninitializeTestVideoRendering();

	//Frame Buffer Rendering
	int initializeTestFBORendering();
	void displayTestFBORendering();
	void updateTestFBORendering();
	void uninitializeTestFBORendering();

	//Atmospherice scattering
	int initializeTestAtmosphereicScatteringendering();
	void displayTestAtmosphereicScatteringRendering();
	void updateTestAtmosphereicScatteringRendering();
	void uninitializeTestAtmosphereicScatteringRendering();


	//Cubemap
	int initializeTestCubeMap();
	void displayTestCubeMap();
	void updateTestCubeMap();
	void uninitializeTestCubeMap();

	//Ocean
	int initializeTestOcean();
	void displayTestOcean(float,float);
	void updateTestOcean();
	void uninitializeTestOcean();

	//cloth
	int initializeTestClothRendering();
	void displayTestClothRendering();
	void updateTestClothRendering();
	void uninitializeTestClothRendering();

	// Terrain
	int initializeTestTerrain();
	void displayTestTerrain();
	void updateTestTerrain();
	void uninitializeTestTerrain();

	//Water
	int initializeTestWaterRendering();
	void displayTestWaterRendering();
	void updateTestWaterRendering();
	void uninitializeTestWaterRendering();

	int initializeTestObjModelLoading();
	void uninitializeTestObjModelLoading();
	void displayTestObjModelLoading();

	void updateTestObjModelLoading();
};


