#pragma once
#include "../../Utility/Headers/ShaderManager.h"
#include "../../Source/Common.h"



#define REFLECTION_FBO_WIDTH 1920
#define REFLECTION_FBO_HEIGHT 1080

#define REFRACTION_FBO_WIDTH 1920
#define REFRACTION_FBO_HEIGHT 1080

#define WATER_WAVE_SPEED 0.002f


class WaterShader {
private:

	bool initializeReflectionFBO(GLint reflectionTextureWidth, GLint reflectionTextureHeight);
	bool initializeRefractionFBO(GLint refractionTextureWidth, GLint refractionTextureHeight);
	void renderRefraction(GLint textureWidth, GLint textureHeight, vec3 camPos, vec3 camCentre, vec3 camUP);
	void renderReflection(GLint textureWidth, GLint textureHeight, vec3 campos, vec3 camcenter, vec3 camUp, void (*belowWater)(mat4));


	void renderAboveWaterScene(bool bDisplayReflection, vec3 camPos, vec3 camCentre, vec3 camUP, void(*belowWater)(mat4));
	void renderBelowWaterScene(vec3 campos, vec3 camcenter, vec3 camUp);

	void renderWaterbedQuad(vec3 campos, vec3 camcenter, vec3 camUp);
	void displayWaterQuadShader(vec3 lightColor, vec3  lightPosition, vec3 campos, vec3 camcenter, vec3 camUp, float moveFactor, GLuint fbo_texture_reflection, GLuint fbo_texture_refraction, GLuint waterDDVTexture, GLuint waterNormalMapTexture);


	GLuint fbo_reflection;
	GLuint rbo_reflection;
	GLuint fbo_texture_reflection;

	GLuint fbo_refraction;
	GLuint rbo_texture_refraction;
	GLuint fbo_texture_refraction;

	ShaderManager shaderProgramObjectWaterBedQuad;
	ShaderManager shaderProgramObjectWaterQuad;


public:
	int initializeWaterShaderProgram();
	void displayWaterShader(mat4 modelMatrix, vec3 campos, vec3 camcenter, vec3 camUp, vec3 lightColor, vec3 lightPosition, float moveFactor, GLuint waterDDVTexture, GLuint waterNormalMapTexture, void (*belowWater)(mat4));
	void deleteShaderProgramObject();
	void updateShaderProramObject();
};
