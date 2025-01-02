#pragma once

#include "../../Source/Globals.h"



// change spelling to Framebuffer also change spelling of textHgt textWdh
class FrameBufferWithDepth {
private:

public:
	// variable declarations
	GLuint fbo;
	GLuint colorTexture;
	GLuint depthTexture;
	GLuint textureWidth;
	GLuint textureHeight;
	bool isError;

	// function declarations
	FrameBufferWithDepth(GLint textureWidth, GLint textureHeight, const char* fboName = "fbo");

	void start();

	void end();

	void resize(int width, int height);
};
