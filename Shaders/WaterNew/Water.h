#pragma once
#include "../../Utility/Headers/ShaderManager.h"
#include "../../Source/Common.h"

#define PATCH_SIZE  64

class WaterFrameBuffers {
private:
	ShaderManager shaderProgramObject;
private:
	static const GLuint REFLECTION_WIDTH = 1920;
	static const GLuint REFLECTION_HEIGHT = 1080;

	const static GLuint REFRACTION_WIDTH = 1920;
	const static GLuint REFRACTION_HEIGHT = 1080;

	GLuint reflectionFrameBuffer;
	GLuint reflectionTexture;
	GLuint reflectionDepthBuffer;

	GLuint refractionFrameBuffer;
	GLuint refractionTexture;
	GLuint refractionDepthTexture;

	GLfloat moveFactor;
	ShaderManager shaderProgramObjectWaterQuad;

	GLuint textureSamplerUniform_waterReflection;
	GLuint textureSamplerUniform_waterRefraction;
	GLuint textureSamplerUniform_waterDepth;
	GLuint modelMatrixUniform_water;
	GLuint viewMatrixUniform_water;
	GLuint projectionMatrixUniform_water;
	GLuint lightingEnabledUniform_water;
	GLuint textureSamplerUniform_waterDUDVMap;
	GLuint watermovingFactorUniform;
	GLuint textureSamplerUniform_waterNormal;
	GLuint watercamerapositonUniform;
	GLuint waterlightPositionUniform;
	GLuint waterlightColorUniform;
	GLuint waterrippleSpeedUniform;
	GLuint waterrippleAmplitudeUniform;


public:

	WaterFrameBuffers();
	int initializeWaterShaderProgram();
	void cleanUp();
	void bindReflectionFrameBuffer();
	void bindRefractionFrameBuffer();
	void unbindCurrentFrameBuffer(GLuint width, GLuint height);
	GLfloat getMoveFactor();
	GLuint getReflectionTexture();
	GLuint getRefractionTexture();
	GLuint getRefractionDepthTexture();

	void initialiseReflectionFrameBuffer();
	void initialiseRefractionFrameBuffer();
	void bindFrameBuffer(GLuint frameBuffer, GLuint width, GLuint height);
	void createFrameBuffer(GLuint* frameBuffer);
	void createTextureAttachment(GLuint width, GLuint height, GLuint* texture);
	void createDepthTextureAttachment(GLuint width, GLuint height, GLuint* texture);
	void createDepthBufferAttachment(GLuint width, GLuint height, GLuint* depthBuffer);

	void draw_water(GLuint  waterDDVTexture, GLuint waterNormalMapTexture, mat4 modelMatrix,mat4 viewMatrix,vec3);
	void getUniform_water();
	void initialization_water();

};