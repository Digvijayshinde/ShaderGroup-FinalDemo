#pragma once
#include "Water.h"
#include "../../Test/Headers/TestCommon.h"	

extern Test test;


int WaterShader::initializeWaterShaderProgram() {

	std::vector<std::string> shaderName;
	shaderName.push_back("Shaders\\Water\\WaterbedQuad.vert");
	shaderName.push_back("Shaders\\Water\\WaterbedQuad.frag");
	int retVal = shaderProgramObjectWaterBedQuad.loadShaderAndCompileShader(shaderName);
	if (retVal != 0) {
		PrintLog("loading ofwater bed quad fail\n");
		return -1;
	}

	shaderName.clear();

	shaderName.push_back("Shaders\\Water\\WaterQuad.vert");
	shaderName.push_back("Shaders\\Water\\WaterQuad.frag");
	 retVal = shaderProgramObjectWaterQuad.loadShaderAndCompileShader(shaderName);
	if (retVal != 0) {
		PrintLog("loading of water quad fail\n");
		return -1;
	} 


	if (initializeReflectionFBO(REFLECTION_FBO_WIDTH, REFLECTION_FBO_HEIGHT) == FALSE)
	{
		return(-1);
	}

	if (initializeRefractionFBO(REFRACTION_FBO_WIDTH, REFRACTION_FBO_HEIGHT) == FALSE)
	{
		return(-1);

	}
	return 0;

}


bool  WaterShader::initializeReflectionFBO(GLint reflectionTextureWidth, GLint reflectionTextureHeight)
{
	// code
	// STEPS : 
	//Check Available Render Buffer Size
	int maxRenderbufferSize;

	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);

	if (maxRenderbufferSize < reflectionTextureWidth || maxRenderbufferSize < reflectionTextureHeight)
	{
		PrintLog("Insufficient Render Buffer Size \n");
		return false;
	}

	// Create Framebuffer Object
	glGenFramebuffers(1, &fbo_reflection);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_reflection);

	// Create Renderbuffer Object
	glGenRenderbuffers(1, &rbo_reflection);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_reflection);

	// Where To Keep This Renderbuffer And What Will the Format Of Renderbuffer
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, reflectionTextureWidth, reflectionTextureHeight);  // Nothing to do with depth

	// Create Empty Texture For Upcoming Target Scene
	glGenTextures(1, &fbo_texture_reflection);
	glBindTexture(GL_TEXTURE_2D, fbo_texture_reflection);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, reflectionTextureWidth, reflectionTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	// Give Above Texture To FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture_reflection, 0);

	// Give Renderbuffer TO FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_reflection);

	// Check Wheather The Framebuffer Is Created Successfully or Not
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (result != GL_FRAMEBUFFER_COMPLETE)
	{
		PrintLog("Reflection Framebuffer is not Complete \n");
		return FALSE;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return(TRUE);
}

bool WaterShader:: initializeRefractionFBO(GLint refractionTextureWidth, GLint refractionTextureHeight)
{
	// code
	// STEPS : 
	//Check Available Render Buffer Size
	int maxRenderbufferSize;

	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);

	if (maxRenderbufferSize < refractionTextureWidth || maxRenderbufferSize < refractionTextureHeight)
	{
		PrintLog("Insufficient Render Buffer Size \n");
		return false;
	}

	// Create Framebuffer Object
	glGenFramebuffers(1, &fbo_refraction);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_refraction);

	// Create Empty Texture For Upcoming Target Scene
	glGenTextures(1, &fbo_texture_refraction);
	glBindTexture(GL_TEXTURE_2D, fbo_texture_refraction);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, refractionTextureWidth, refractionTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	// Give Above Texture To FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture_refraction, 0);

	// Create Empty Texture For Upcoming Target Scene
	glGenTextures(1, &rbo_texture_refraction);
	glBindTexture(GL_TEXTURE_2D, rbo_texture_refraction);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, refractionTextureWidth, refractionTextureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// Give Above Texture To FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rbo_texture_refraction, 0);

	// Check Wheather The Framebuffer Is Created Successfully or Not
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (result != GL_FRAMEBUFFER_COMPLETE)
	{
		PrintLog("Refraction Framebuffer is not Complete \n");
		return FALSE;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return(TRUE);
}

void WaterShader ::displayWaterQuadShader(vec3 lightColor, vec3 lightPosition, vec3  campos, vec3 camcenter, vec3 camUp, float  moveFactor, GLuint fbo_texture_reflection, GLuint fbo_texture_refraction, GLuint  waterDDVTexture, GLuint waterNormalMapTexture) {


	shaderProgramObjectWaterQuad.useProgram();
	
	mat4 viewMatrix = vmath::lookat(campos, camcenter, camUp);
	mat4 modelMatrix= vmath::scale(50.0f,50.0f,50.0f);
	shaderProgramObjectWaterQuad.setMat4("u_modelMatrix", modelMatrix);
	shaderProgramObjectWaterQuad.setMat4("u_viewMatrix", viewMatrix);
	shaderProgramObjectWaterQuad.setMat4("u_projectionMatrix", prespectiveProjectionMatrix);
	shaderProgramObjectWaterQuad.setVec3("u_cameraPosition", campos);
	shaderProgramObjectWaterQuad.setVec3("u_lightColor", lightColor);
	shaderProgramObjectWaterQuad.setVec3("u_lightPosition", lightPosition);
	shaderProgramObjectWaterQuad.setFloat("u_moveFactorOffset", moveFactor);

	//activating texture 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo_texture_reflection);
	shaderProgramObjectWaterQuad.setInt("u_reflectionTextureSampler", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fbo_texture_refraction);
	shaderProgramObjectWaterQuad.setInt("u_refractionTextureSampler", 1);


	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, waterDDVTexture);
	shaderProgramObjectWaterQuad.setInt("u_waterDUDVMapTextureSampler", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, waterNormalMapTexture);
	shaderProgramObjectWaterQuad.setInt("u_waterNormalMapTextureSampler", 3);

	renderes->renderQuad();
	shaderProgramObjectWaterQuad.unUseProgram();

}

void WaterShader::displayWaterShader(mat4 modelMatrix,vec3 campos,vec3 camcenter,vec3 camUp,vec3 lightColor, vec3 lightPosition,float moveFactor,GLuint waterDDVTexture,GLuint waterNormalMapTexture,void (*aboveWater)(mat4)) {

	renderReflection(REFLECTION_FBO_WIDTH, REFLECTION_FBO_HEIGHT, campos, camcenter, camUp,aboveWater);
	renderRefraction(REFRACTION_FBO_WIDTH, REFRACTION_FBO_HEIGHT, campos, camcenter, camUp);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, gWinWidth, gWinHeight);

	displayWaterQuadShader(lightColor,lightPosition, campos, camcenter, camUp, moveFactor,fbo_texture_reflection,fbo_texture_refraction, waterDDVTexture, waterNormalMapTexture);

	renderAboveWaterScene(false, campos, camcenter, camUp, aboveWater);
	renderBelowWaterScene(campos, camcenter, camUp);

}

void WaterShader ::renderReflection(GLint textureWidth, GLint textureHeight, vec3 campos, vec3 camcenter, vec3 camUp, void (*aboveWater)(mat4))
{
	// Code
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_reflection);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, textureWidth, textureHeight);

	// Render Scene To Be Reflected
	renderAboveWaterScene(true, campos, camcenter, camUp, aboveWater);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
void WaterShader :: renderAboveWaterScene(bool bDisplayReflection, vec3 campos, vec3 camCentre, vec3 camUP, void (*aboveWater)(mat4))
{


	// Variable declaration
	vec3 changedCameraPosition = vec3(0.0f, 0.0f, 0.0f);

	changedCameraPosition = campos;
	if (bDisplayReflection == true)
	{
		changedCameraPosition[1] = changedCameraPosition[1] * -1.0f;  // Invert Y Of Camera If You Are Showing The Reflection.
	}
	mat4 viewMatrix = vmath::lookat(changedCameraPosition, camCentre, camUP);
	(*aboveWater)(viewMatrix);

	test.displayTestTerrain();


}
void WaterShader::renderRefraction(GLint textureWidth, GLint textureHeight, vec3 camPos, vec3 camCentre, vec3 camUP)
{

	// Code
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_refraction);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, textureWidth, textureHeight);

	// Render Scene To Be Refracted
	renderBelowWaterScene(camPos, camCentre, camUP);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void  WaterShader::renderBelowWaterScene(vec3 campos, vec3 camcenter, vec3 camUp)
{
	renderWaterbedQuad(campos, camcenter, camUp);
	//Add here any thing below the water

}

void WaterShader::renderWaterbedQuad(vec3 campos,vec3 camcenter,vec3 camUp) {


	shaderProgramObjectWaterBedQuad.useProgram();

	mat4 viewMatrix = vmath::lookat(campos, camcenter, camUp);
	mat4 translationMatrix = vmath::translate(0.0f, -0.2f, 0.0f);
	mat4 scaleMatrix = vmath::scale(50.0f, 50.0f, 50.0f);

	mat4 modelMatrix = translationMatrix * scaleMatrix;

	shaderProgramObjectWaterBedQuad.setMat4("u_modelMatrix", modelMatrix);
	shaderProgramObjectWaterBedQuad.setMat4("u_viewMatrix", viewMatrix);
	shaderProgramObjectWaterBedQuad.setMat4("u_projectionMatrix", prespectiveProjectionMatrix);
	shaderProgramObjectWaterBedQuad.setVec3("a_color",{1.0f,0.0f,0.0f});

	renderes->renderQuad();
	shaderProgramObjectWaterBedQuad.unUseProgram();

}

void WaterShader::updateShaderProramObject()
{
}
void WaterShader::deleteShaderProgramObject() {
	shaderProgramObjectWaterBedQuad.uninitializeShader();
	shaderProgramObjectWaterQuad.uninitializeShader();
}


