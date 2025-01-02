#include "Water.h"
#include "../../Test/Headers/TestCommon.h"			//For testing purpose

extern GLuint shaderProgramObject_water;

extern mat4 gPerspectiveProjectionMatrix;
extern vec3 cameraPos;

#define WAVE_SPEED  0.00005f

bool is_ripple_on=false;

extern float deltaTime;

// FBO related variables
GLuint fbo_water_reflection;
GLuint rbo_water_reflection;
GLuint fbo_texture_water_reflection;

GLuint fbo_water_refraction;
GLuint rbo_water_refraction;
GLuint fbo_texture_water_refraction;


extern Test test;

	WaterFrameBuffers::WaterFrameBuffers():moveFactor(0.0f) {//call when loading the game
		
	}

	int WaterFrameBuffers::initializeWaterShaderProgram() {

		std::vector<std::string> shaderName;
		shaderName.clear();

		shaderName.push_back("Shaders\\WaterNew\\VertexShader_Water.vert");
		shaderName.push_back("Shaders\\WaterNew\\FragmentShader_Water.frag");
		int retVal = shaderProgramObjectWaterQuad.loadShaderAndCompileShader(shaderName);
		if (retVal != 0) {
			PrintLog("loading of water quad fail\n");
			return -1;
		}
		return 0;
	}



	void WaterFrameBuffers::cleanUp() {//call when closing the game
		// glDeleteFramebuffers(reflectionFrameBuffer);
		// glDeleteTextures(reflectionTexture);
		// glDeleteRenderbuffers(reflectionDepthBuffer);
		// glDeleteFramebuffers(refractionFrameBuffer);
		// glDeleteTextures(refractionTexture);
		// glDeleteTextures(refractionDepthTexture);
	}
   
	void WaterFrameBuffers::bindReflectionFrameBuffer() {//call before rendering to this FBO
		bindFrameBuffer(reflectionFrameBuffer,REFLECTION_WIDTH,REFLECTION_HEIGHT);
	}
	
	void WaterFrameBuffers::bindRefractionFrameBuffer() {//call before rendering to this FBO
		bindFrameBuffer(refractionFrameBuffer,REFRACTION_WIDTH,REFRACTION_HEIGHT);
	}
	
	void WaterFrameBuffers::unbindCurrentFrameBuffer(GLuint width,GLuint height) {//call to switch to default frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0,width,height);
	}

	
	GLfloat WaterFrameBuffers::getMoveFactor() {//get the resulting texture
		moveFactor+= WAVE_SPEED;
		return moveFactor;
	}

	GLuint WaterFrameBuffers::getReflectionTexture() {//get the resulting texture
		return reflectionTexture;
	}
	
	GLuint WaterFrameBuffers::getRefractionTexture() {//get the resulting texture
		return refractionTexture;
	}
	
	GLuint WaterFrameBuffers::getRefractionDepthTexture(){//get the resulting depth texture
		return refractionDepthTexture;
	}


	void WaterFrameBuffers::initialiseReflectionFrameBuffer() {
		createFrameBuffer(&reflectionFrameBuffer);
		createTextureAttachment(REFLECTION_WIDTH,REFLECTION_HEIGHT,&reflectionTexture);
		createDepthBufferAttachment(REFLECTION_WIDTH,REFLECTION_HEIGHT,&reflectionDepthBuffer);
		unbindCurrentFrameBuffer(gWinWidth,gWinHeight);
	}
	
	void WaterFrameBuffers::initialiseRefractionFrameBuffer() {
		createFrameBuffer(&refractionFrameBuffer);
		createTextureAttachment(REFRACTION_WIDTH,REFRACTION_HEIGHT,&refractionTexture);
		createDepthTextureAttachment(REFRACTION_WIDTH,REFRACTION_HEIGHT,&refractionDepthTexture);
		unbindCurrentFrameBuffer(gWinWidth, gWinHeight);
	}
	
	void WaterFrameBuffers::bindFrameBuffer(GLuint frameBuffer, GLuint width, GLuint height){
		glBindTexture(GL_TEXTURE_2D, 0);//To make sure the texture isn't bound
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glViewport(0, 0, width, height);
	}

	void WaterFrameBuffers::createFrameBuffer(GLuint *frameBuffer) {
		glGenFramebuffers(1,frameBuffer);
		//generate name for frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, *frameBuffer);
		//create the framebuffer
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		//indicate that we will always render to color attachment 0
	}

	void WaterFrameBuffers::createTextureAttachment( GLuint width, GLuint height,GLuint *texture) {
		glGenTextures(1,texture);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
				0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				*texture, 0);
		
	}
	
	void WaterFrameBuffers::createDepthTextureAttachment(GLuint width, GLuint height,GLuint *texture){
		glGenTextures(1,texture);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height,
				0, GL_DEPTH_COMPONENT, GL_FLOAT,NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				*texture, 0);
	}

	void WaterFrameBuffers::createDepthBufferAttachment(GLuint width, GLuint height,GLuint * depthBuffer) {
		glGenRenderbuffers(1,depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, *depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width,
				height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				GL_RENDERBUFFER, *depthBuffer);
	}


void WaterFrameBuffers::getUniform_water() {
	// get MVP uniform location
	modelMatrixUniform_water = glGetUniformLocation(shaderProgramObjectWaterQuad.getShaderProgramObject(), "u_modelMatrix");
	viewMatrixUniform_water = glGetUniformLocation(shaderProgramObjectWaterQuad.getShaderProgramObject(), "u_viewMatrix");
	projectionMatrixUniform_water = glGetUniformLocation(shaderProgramObjectWaterQuad.getShaderProgramObject(), "u_projectionMatrix");
	textureSamplerUniform_waterReflection = glGetUniformLocation(shaderProgramObjectWaterQuad.getShaderProgramObject(), "sampler_textureReflection");
	textureSamplerUniform_waterRefraction = glGetUniformLocation(shaderProgramObjectWaterQuad.getShaderProgramObject(), "sampler_textureRefraction");
	textureSamplerUniform_waterDUDVMap = glGetUniformLocation(shaderProgramObjectWaterQuad.getShaderProgramObject(), "sampler_textureDUDVMap");
	textureSamplerUniform_waterNormal = glGetUniformLocation(shaderProgramObjectWaterQuad.getShaderProgramObject(), "sampler_texturewaterNormalMap");
	textureSamplerUniform_waterDepth = glGetUniformLocation(shaderProgramObjectWaterQuad.getShaderProgramObject(), "sampler_texturedepthMap");
	watercamerapositonUniform = glGetUniformLocation(shaderProgramObjectWaterQuad.getShaderProgramObject(), "cameraPosition");
	watermovingFactorUniform = glGetUniformLocation(shaderProgramObjectWaterQuad.getShaderProgramObject(), "moveFactor");
	waterlightPositionUniform = glGetUniformLocation(shaderProgramObjectWaterQuad.getShaderProgramObject(), "lightPosition");
	waterlightColorUniform = glGetUniformLocation(shaderProgramObjectWaterQuad.getShaderProgramObject(), "lightColor");
	waterrippleSpeedUniform = glGetUniformLocation(shaderProgramObjectWaterQuad.getShaderProgramObject(), "uTime");
	waterrippleAmplitudeUniform = glGetUniformLocation(shaderProgramObjectWaterQuad.getShaderProgramObject(), "amplitude");
}


void WaterFrameBuffers::initialization_water()
{
	initializeWaterShaderProgram();
	initialiseReflectionFrameBuffer();
	initialiseRefractionFrameBuffer();
    getUniform_water();
}


void WaterFrameBuffers::draw_water(GLuint  waterDDVTexture, GLuint waterNormalMapTexture, mat4 modelMatrix,mat4 viewMatrix,vec3 camPos)
{
   // variable declarations

	// function declarations

	shaderProgramObjectWaterQuad.useProgram();

	glUniformMatrix4fv(modelMatrixUniform_water, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_water, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_water, 1, GL_FALSE, prespectiveProjectionMatrix);
    glUniform1f(watermovingFactorUniform, this->getMoveFactor());
	glUniform3fv(watercamerapositonUniform,1, camPos);
	glUniform3fv(waterlightPositionUniform,1, vec3(-515,10200,911));
	glUniform3fv(waterlightColorUniform,1,vec3(0.9f,0.9f,1.0f));
	

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->getReflectionTexture());
	glUniform1i(textureSamplerUniform_waterReflection, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->getRefractionTexture());
	glUniform1i(textureSamplerUniform_waterRefraction, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, waterNormalMapTexture);
	glUniform1i(textureSamplerUniform_waterNormal, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, waterDDVTexture);
	glUniform1i(textureSamplerUniform_waterDUDVMap, 3);

	
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, this->getRefractionDepthTexture());
	glUniform1i(textureSamplerUniform_waterDepth, 4);

	//glBindTexture(GL_TEXTURE_2D, 0);
	
	renderes->renderQuad();
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);

}