#pragma once
#ifndef PARTICLE_SYSTEM_RENDERER_H
#define PARTICLE_SYSTEM_RENDERER_H

#include "../../Utility/Headers/ShaderManager.h"
#include "../../Source/Common.h"
#include <cuda_gl_interop.h>
#include <cuda_runtime.h>

//default value for simulation time step
#define TIME_STEP 0.01f

//default damping constant
#define DAMP 1.0

enum VboType
{
	POSITION_OBJECT = 0,
	NORMAL_OBJECT = 1
};

// externally defined methods for CUDA implementation 
extern void InitCuda(const int size);
extern void UploadCuda(float* positions, float* positions_old, const int size);
extern void VerletCuda(float4* pos_vbo, float4* nor_vbo, float* positions, float* positions_old, const int size, const int& side, const float& stiffness, const float& damp, const float& inverse_mass, const int& coll_primitives);
extern void ResetCuda();


class ClothRenderingShader {
private:
	ShaderManager shaderProgramObject;
	int BuildCloth(int nb, float stiffness = 3000.0f, float damp = 1.0f, float inv_mass = 1.0f);
	void ResetClothParticleSystem(void);
	void ResetClothMemoryAndBuffers(void);
	void InitBuffers(void);
	int  InitInteropVBO(void);
	unsigned int nearest_pow(unsigned int num);


	void DeleteCudaVBO(void);
	void ResetBuffers(void);
	void  NextNeigh(int n, int& i, int& j);
	void VerletCPU();

public:

	std::vector<float> positions; //current position of the particles
	std::vector<float> positionsOld; //previous position of the particles
	std::vector<float> normals; //normal of each particle

	std::vector<float> texcoords;	// 2D texture coordinates of each particle on the cloth (used for rendering purposes)
	int nbSprings; //number of springs

	std::vector<unsigned short> faces; //three short for each face, dim = 3 * nbfaces; it used as index buffer only for rendering phase

	float stretchStiffness;
	//damping constant
	float damp = DAMP;
	//inverse of the mass of each particle in the system (if 0, it means the particle has infinite mass and can not move)
	float inverseMass;

	//GLSL data structures
	int textureSize; //size of textures which stores the state of the particle system

	float particleTimeStep ;

	GLuint vaoFabric;
	GLuint vboPositionFabric;
	GLuint vboNormalFabric;
	GLuint vboFabricElements;

	//uniforms
	GLuint fabricMVMatrixUniform;
	GLuint fabricProjectionMatrixUniform;
	GLuint fabricColorUniform;

	enum ComputationType { CPU = 0, CUDA, OPENCL, NOCOMPUTATION };
	enum CollidingObjects { COLLIDING_NONE, COLLIDING_SPHERE, COLLIDING_PLANE };

	ComputationType computation = CPU;
	CollidingObjects collidingObject = COLLIDING_NONE;

	 BOOL doesInitializationDone;
	 int numberOfSimulationSteps;

	//global CUDA variables
	 cudaError_t cudaResult;
	 cudaGraphicsResource* cuda_graphics_pos_resource = NULL;
	 cudaGraphicsResource* cuda_graphics_norm_resource = NULL;

	 ClothRenderingShader() {
		 inverseMass = 1.0f;
		 stretchStiffness = 1.0f;
		 particleTimeStep = TIME_STEP;
		 doesInitializationDone = FALSE;
		 numberOfSimulationSteps = 1;
		 cuda_graphics_pos_resource = NULL;
		 cuda_graphics_norm_resource = NULL;
	 }

	int initializeClothRenderingShaderProgram();
	void useClothRenderingShaderProgram();
	void displayClothRenderingShader(mat4,mat4,vec4);
	void updateClothRenderingShader();
	void unUseClothRenderingShaderProgram();
	void deleteShaderProgramObject();
	void TimeStep(void);

};
#endif