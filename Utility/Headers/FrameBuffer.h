#pragma once
#include "../../Source/Globals.h"

bool CreateFBO(GLint textureWidth, GLint textureHeight, FrameBuffer& frameBuffer, int numberOfAttachments = 1);

void resizeFBO(int width, int height);

///* Frame Buffer Object */
//typedef struct _tagFramebuffer
//{
//	GLuint fbo;
//
//	GLuint width;
//	GLuint height;
//
//	/*
//	* If there is only one attachment in the frame buffer then texture_color_buffer is used otherwise color_attachments array is created based
//	* on how many color attachments are there in the framebuffer
//	*/
//
//	//GLuint texture_color_buffer;
//
//	/*
//		Color attachments buffers GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 etc.
//	*/
//	GLuint* color_attachments;
//
//	/*
//		How many color buffer attachments are there in the framebuffer
//	*/
//	GLuint count_color_attachments;
//
//	/*
//		this boolean indicates weather the framebuffer has depth buffer or not
//	*/
//	bool is_depth_buffer;
//
//	/*
//		boolean indicates weather if the depther buffer is present then it is texture buffer or render buffer
//	*/
//	bool is_depth_texture_buffer;
//
//	/*
//		Depth buffer as a texture attachment or render buffer attachment
//	*/
//	GLuint depth_buffer;
//
//}FramebufferWithDepth;
//
//FramebufferWithDepth* GenerateFrameBufferObject(GLuint width, GLuint height, GLuint count_color_attachments = 1, bool is_depth = false, bool is_depth_texture_buffer = false);
//
//void resizeFBOWithDepth(int width, int height);

