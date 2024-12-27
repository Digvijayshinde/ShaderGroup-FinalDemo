#pragma once
#include "../../Source/Globals.h"
#include "../../Utility/Headers/Ocean/3Dmath.h"
#include "../../Utility/Headers/Ocean/OceanCamera.h"
#include "../../Utility/Headers/Ocean/OceanQuadMesh.h"
#include "../../Utility/Headers/Ocean/OpenGLMesh.h"
#include <random>

// tweakables
#define DISP_MAP_SIZE		512					// 1024 max
#define MESH_SIZE			256					// [64, 256] (or calculate index count for other levels)
#define GRAV_ACCELERATION	9.81f				// m/s^2
#define PATCH_SIZE			20.0f				// m
#define FURTHEST_COVER		8					// full ocean size = PATCH_SIZE * (1 << FURTHEST_COVER)
#define MAX_COVERAGE		64.0f				// pixel limit for a distant patch to be rendered
#define WIND_DIRECTION		{ -0.4f, -0.9f }
#define WIND_SPEED			6.5f				// m/s
#define AMPLITUDE_CONSTANT	(0.45f * 1e-3f)		// for the (modified) Phillips spectrum

static const int IndexCounts[] = {
	0,
	0,
	0,
	0,
	0,
	0,
	961920,		// 64x64
	3705084,	// 128x128
	14500728	// 256x256
};

int initializeOceanShader(void);
void uninitializeOceanShader(void);

int initializeUpdateSpectrumShader(void);
void uninitializeUpdateSpectrumShader(void);

int initializeFourierFFTShader(void);
void uninitializeFourierFFTShader(void);

int initializeDisplacementShader(void);
void uninitializeDisplacementShader(void);

int initializeGradientsShader(void);
void uninitializeGradientsShader(void);

int initializeOceanEffect(void);
void uninitializeOceanEffect(void);

void renderOcean(unsigned int envmap, float alpha, float elapsedtime);

void callbackOceanShader(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
void moveOceanCameraForward(void);
void moveOceanCameraBackward(void);
void updateOceanShader(void);
