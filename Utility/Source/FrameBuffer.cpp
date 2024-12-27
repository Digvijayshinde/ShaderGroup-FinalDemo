#include "../Headers/FrameBuffer.h"

bool CreateFBO(GLint textureWidth, GLint textureHeight, FrameBuffer& frameBuffer, int numberOfAttachments)
{
	//variable declarations
	int maxRenderbufferSize;
	//FrameBuffer frameBuffer;

	//code
	//Step 1: Check available render buffer size
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);
	if (maxRenderbufferSize < textureWidth || maxRenderbufferSize < textureHeight)
	{
		PrintLog("Insufficient render buffer size\n");
		return (false);
	}

	//step 2: create framebuffer object
	glGenFramebuffers(1, &frameBuffer.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.fbo);

	//step 3: create render buffer object
	glGenRenderbuffers(1, &frameBuffer.rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, frameBuffer.rbo);

	for (int i = 0; i < numberOfAttachments; i++)
	{
		//Create Empty Texure for upcoming target scene
		glGenTextures(1, &frameBuffer.fboTexture[i]);
		glBindTexture(GL_TEXTURE_2D, frameBuffer.fboTexture[i]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
		glGenerateMipmap(GL_TEXTURE_2D);

		//give above texture to fbo
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, frameBuffer.fboTexture[i], 0);
	}

	//storage and format of the render buffer
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, textureWidth, textureHeight);

	//give render buffer to fbo
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBuffer.rbo);

	//check framebuffer created successfully or not
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result != GL_FRAMEBUFFER_COMPLETE)
	{
		PrintLog("Framebuffer is not complete.\n");
		return (false);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//when franbuffer gets unbind along with that render buffer and texture buffer will get unbind no need to right them separately

	return (true);
}

void resizeFBO(int width, int height)
{
	if (height == 0) //to avoid possible divide by zero in future code
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	prespectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 10000.0f);
}

//FramebufferWithDepth* GenerateFrameBufferObject(
//	GLuint width,
//	GLuint height,
//	GLuint count_color_attachments,
//	bool is_depth,
//	bool is_depth_texture_buffer)
//{
//	FramebufferWithDepth* framebuffer = new FramebufferWithDepth;
//	framebuffer->width = width;
//	framebuffer->height = height;
//
//	glGenFramebuffers(1, &framebuffer->fbo);
//	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->fbo);
//
//	if (is_depth)
//	{
//		framebuffer->is_depth_buffer = true;
//
//		if (is_depth_texture_buffer)
//		{
//			framebuffer->is_depth_texture_buffer = true;
//
//			/*
//				Depth is created as texture buffer
//			*/
//			glGenTextures(1, &framebuffer->depth_buffer);
//			glBindTexture(GL_TEXTURE_2D, framebuffer->depth_buffer);
//			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, framebuffer->depth_buffer, 0);
//		}
//		else
//		{
//			framebuffer->is_depth_texture_buffer = false;
//			/*
//				* Depth is created as render buffer
//			*/
//
//			glGenRenderbuffers(1, &framebuffer->depth_buffer);
//			glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->depth_buffer);
//			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
//			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer->depth_buffer);
//		}
//	}
//
//	if (count_color_attachments >= 1)
//	{
//		framebuffer->color_attachments = new GLuint[count_color_attachments];
//		GLuint* color_attachments_flag = new GLuint[count_color_attachments];
//		glGenTextures(count_color_attachments, framebuffer->color_attachments);
//
//		for (GLuint index = 0; index < count_color_attachments; index++)
//		{
//			glBindTexture(GL_TEXTURE_2D, framebuffer->color_attachments[index]);
//			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//			glGenerateMipmap(GL_TEXTURE_2D);
//
//			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, framebuffer->color_attachments[index], 0);
//
//			color_attachments_flag[index] = GL_COLOR_ATTACHMENT0 + index;
//		}
//
//		glDrawBuffers(count_color_attachments, color_attachments_flag);
//		delete[] color_attachments_flag;
//	}
//
//	framebuffer->count_color_attachments = count_color_attachments;
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	return framebuffer;
//}
//
//void resizeFBOWithDepth(int width, int height)
//{
//	if (height == 0) //to avoid possible divide by zero in future code
//	{
//		height = 1;
//	}
//
//	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
//
//	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 10000.0f);
//}
