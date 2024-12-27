#pragma once	
#include "../../Source/Common.h"

class Renderers {
	
	//code
public:

	Renderers() {};

	int initializeQuadRenderer();
	void renderQuad();
	void unitializeQuad();

	int initializeCubeRenderer();
	void renderCube();
	void unitializeCube();

	int initializePyramidRenderer();
	void renderPyramid();
	void unitializePyramid();

	int initializeSphereRenderer();
	void renderSphere();
	void unitializeSphere();

};