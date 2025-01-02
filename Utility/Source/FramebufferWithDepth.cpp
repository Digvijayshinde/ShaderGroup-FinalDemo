#include "../Headers/FramebufferWithDepth.h"

// this should return fbo it also teake texture input fbo input
FrameBufferWithDepth::FrameBufferWithDepth(GLint textWdh, GLint textHgt, const char* fboName)
{
	// variable declarations
	int maxRenderBufferSize;

	// default value 0f isError should be false
	isError = false;

	textureWidth = textWdh;
	textureHeight = textHgt;

	// code
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize);
	if (maxRenderBufferSize < textureWidth || maxRenderBufferSize < textureHeight)
	{
		isError = true;
		exit(0);
	}

	// create FBO
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// need to read rbo
	//glGenRenderbuffers(1, &rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	// where to keep this render buffer and what will be formate of this render buffer(storage and formate of render buffer)
	// ????
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,
		textureWidth, textureHeight);

	// create empty texture
	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);

	// why required below ? check in our framework also
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// added GL_RGBA16F GL_RGBA GL_FLOAT for HDR(float framebuffer)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	//last para: full window texture

	//GL_UNSIGNED_SHORT_5_6_5 this is matching with GL_DEPTH_COMPONENT16
	//for creating a texture take  5 bytes red  6 byte green and 5 byte blue
	//for the better quality of texture use this 565 there is 4444(RGBA)also
	//here 6 value for green is more beacuse green is most adjustable to eye rather that red and blue hence it has gretaer value

	// give texture to fbo

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
	//2 para:  final location of texture
	//3  texture khute aahe currently format and location of memeory tetxure2D
//	5:mipmap level

	glBindTexture(GL_TEXTURE_2D, 0);

	// depth map texture
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		textureWidth, textureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//give rbo to fbo
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	//2 Para :kontyal buffer madhya kadu we say put in depth buffer

	// if created successfully or not
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result != GL_FRAMEBUFFER_COMPLETE)
	{
		isError = true;
		exit(0);
	}
	else
	{
		//printInfoLog(SUCCESS_INFO_LOG, __FILE__, "FBO created successfully :- %s", fboName);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferWithDepth::start()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glViewport(0, 0, textureWidth, textureHeight);

	// perspective matrix change ?
	prespectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)textureWidth / (GLfloat)textureHeight, 0.1f, 500000.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameBufferWithDepth::end()
{
	// code
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	resize(gWinWidth, gWinHeight);
}

void FrameBufferWithDepth::resize(int width, int height)
{
	if (height == 0)
		height = 1;

	glViewport(0, 0, GLsizei(width), GLsizei(height));

	prespectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}
