#pragma once
#include "ClothRendering.h"
#include <inttypes.h>

int ClothRenderingShader::initializeClothRenderingShaderProgram() {

	std::vector<std::string> shaderName;
	shaderName.push_back("Shaders\\ClothRendering\\ClothRendering.vert");
	shaderName.push_back("Shaders\\ClothRendering\\ClothRendering.frag");
	int retVal = shaderProgramObject.loadShaderAndCompileShader(shaderName);
	if (retVal != 0) {
		PrintLog("loading of ClothRendering shader failed\n");
		return -1;
	}

	int deviceCount;

	//code
	cudaError_t cudaResult = cudaGetDeviceCount(&deviceCount);

	if (cudaResult != cudaSuccess)
	{
		PrintLog("cudaGetDeviceCount() failed\n");
		return (-1);
	}
	else if (deviceCount == 0)
	{
		PrintLog("cudaGetDeviceCount() returned 0 devices\n");
		return (-1);
	}
	else
	{
		PrintLog("cudaGetDeviceCount() returned device count: %d\n", deviceCount);
		cudaSetDevice(0); //hard coded 0 i.e. default cuda enabled device (graphics card)
		//cudaGLSetGLDevice(cutGetMaxGflopsDeviceId());
	}

	numberOfSimulationSteps = 4;
	retVal = BuildCloth(32, 55000, 0.5f, 64.0f / (32.0f * 32.0f));

	return 0;

}
void ClothRenderingShader::useClothRenderingShaderProgram() {
	shaderProgramObject.useProgram();

}
void ClothRenderingShader::displayClothRenderingShader(mat4 modelMatrix,mat4 viewMatrix,vec4 color) {


	int size = (int)normals.size() / 4;
	for (int i = 0; i < 4; i++)
	{
		normals[(size - 1) * 4 + i] = normals[(size - 2) * 4 + i];
	}

	shaderProgramObject.setMat4("u_mvMatrix", viewMatrix * modelMatrix);
	shaderProgramObject.setMat4("u_projectionMatrix", prespectiveProjectionMatrix);
	shaderProgramObject.setVec4("u_color", color[0], color[1], color[2],color[3]);

	//bind vao
	glBindVertexArray(vaoFabric);
	glDrawElements(GL_TRIANGLES, (GLsizei)faces.size(), GL_UNSIGNED_SHORT, &faces[0]);
	glBindVertexArray(0);
}
void ClothRenderingShader::updateClothRenderingShader()
{
	TimeStep();
}
void ClothRenderingShader::unUseClothRenderingShaderProgram() {
	shaderProgramObject.unUseProgram();
}
void ClothRenderingShader::deleteShaderProgramObject() {
	shaderProgramObject.uninitializeShader();
}
int ClothRenderingShader :: BuildCloth(int nb, float _stiffness, float _damp, float _inv_mass)
{

	//variable declarations
	int retVal = 0;

	//code
	ResetClothParticleSystem();
	ResetClothMemoryAndBuffers();

	int nbVertices = nb * nb;	// number of vertices

	for (int h = 0; h < nb; h++)
	{
		for (int w = 0; w < nb; w++)
		{
			positions.push_back(w / (float)(nb - 1.0f));	// x
			positions.push_back(0.0f);						// y
			positions.push_back(h / (float)(nb - 1.0f));	// z
			positions.push_back(1.0f);						// w
		}
	}

	int nbfaces = (nb - 1) * (nb - 1) * 2;		// number of triangular faces

	//   i+0,j+0 -- i+0,j+1
	//      |   \     |
	//      |    \    |
	//      |     \   |
	//      |      \  |
	//   i+1,j+0 -- i+1,j+1
	//

	for (int h = 0; h < nb - 1; h++)
	{
		for (int w = 0; w < nb - 1; w++)
		{
			faces.push_back(h * nb + w);
			faces.push_back((h + 1) * nb + w + 1);
			faces.push_back(h * nb + w + 1);

			faces.push_back(h * nb + w);
			faces.push_back((h + 1) * nb + w);
			faces.push_back((h + 1) * nb + w + 1);
		}
	}


	stretchStiffness = _stiffness;
	damp = _damp;
	inverseMass = _inv_mass;

	InitBuffers();
	retVal = InitInteropVBO();

	return (retVal);
}
int  ClothRenderingShader::InitInteropVBO(void)
{
	//variable declarations
	unsigned int size = textureSize * textureSize * 4 * sizeof(float);
	unsigned int elementSize = 3 * 2 * (textureSize - 1) * (textureSize - 1) * sizeof(unsigned short);

	//code
	if (vaoFabric == 0)
	{
		glGenVertexArrays(1, &vaoFabric);
		glGenBuffers(1, &vboPositionFabric);
		glGenBuffers(1, &vboNormalFabric);
		glGenBuffers(1, &vboFabricElements);
	}
	glBindVertexArray(vaoFabric);

	//positions
	glBindBuffer(GL_ARRAY_BUFFER, vboPositionFabric);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(SHADER_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SHADER_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//normals	
	glBindBuffer(GL_ARRAY_BUFFER, vboNormalFabric);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(SHADER_ATTRIBUTE_NORMAL, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SHADER_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (computation == CUDA)
	{
		cudaResult = cudaGraphicsGLRegisterBuffer(&cuda_graphics_pos_resource, vboPositionFabric, cudaGraphicsMapFlagsWriteDiscard);
		if (cudaResult != cudaSuccess)
		{
			PrintLog("cudaGraphicsGLRegisterBuffer() failed for vboPositionFabric with error: %d\n", (int)cudaResult);
			return (-1);
		}

		cudaResult = cudaGraphicsGLRegisterBuffer(&cuda_graphics_norm_resource, vboNormalFabric, cudaGraphicsMapFlagsWriteDiscard);
		if (cudaResult != cudaSuccess)
		{
			PrintLog("cudaGraphicsGLRegisterBuffer() failed for vboNormalFabric with error: %d\n", (int)cudaResult);
			return (-1);
		}

		InitCuda((const int)positions.size());
	}

	glBindVertexArray(0);

	doesInitializationDone = TRUE;

	return (0);
}

 unsigned int ClothRenderingShader:: nearest_pow(unsigned int num)
{
	unsigned int n = num > 0 ? num - 1 : 0;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n++;

	return n;
}

 void ClothRenderingShader::VerletCPU(void)
 {
	 //code
 	 int nbParticles = textureSize * textureSize;
	 vec3 gravity = vec3(0.0f, -9.81f, 0.0f);
	 std::vector<float> positionsNext;
	 positionsNext.assign(nbParticles * 4, 0.0f);

	 for (int index = 0; index < nbParticles; index++)
	 {
		 //fetch data from buffers
		 vec3 pos = vec3(positions[index * 4], positions[index * 4 + 1], positions[index * 4 + 2]);
		 vec3 posOld = vec3(positionsOld[index * 4], positionsOld[index * 4 + 1], positionsOld[index * 4 + 2]);

		 vec3 velocity = (pos - posOld) / particleTimeStep;

		 //use to compute the normal of the vertex corresponding to the particle
		 vec3 normal = vec3(0, 0, 0);
		 vec3 lastDir = vec3(0.0f);
		 int iterator = 0;

		 vec3 force = vec3(0.0f); //force due to springs connected to this particle
		 float invMass = inverseMass;
		 if ((index == 0) || (index <= (textureSize - 1)))	// the first row of particles have infinite mass (they are fixed)
		 {
			 invMass = 0.0f;
		 }

		 int ix = index % textureSize;
		 int iy = index / textureSize;

		 vec3 posRest = vec3(ix / (float)(textureSize - 1), 0, iy / (float)(textureSize - 1)); //rest position of the particle

		 // compute the force due to each spring connected to this particle
		 // in the process, evaluate the normall as well
		 for (int k = 0; k < 12; k++)
		 {
			 int i, j;
			 NextNeigh(k, i, j);

			 if (((iy + i) < 0) || ((iy + i) > (textureSize - 1)))
			 {
				 continue;
			 }

			 if (((ix + j) < 0) || ((ix + j) > (textureSize - 1)))
			 {
				 continue;
			 }

			 int index_neigh = (iy + i) * textureSize + ix + j;
			 vec3 posNeigh = vec3(positions[index_neigh * 4], positions[index_neigh * 4 + 1], positions[index_neigh * 4 + 2]);
			 vec3 diff = posNeigh - pos;

			 vec3 dir = diff;
			 dir = vmath::normalize(dir); // normalized direction of the spring

			 if ((iterator > 0) && (k < 8))
			 {
				 float angle = vmath::dot(dir, lastDir);
				 if (angle > 0)
				 {
					 normal += vmath::cross(lastDir, dir);
				 }
			 }
			 lastDir = dir;	// stored for the next iteration

			 vec3 posNeighRest = vec3((ix + j) / (float)(textureSize - 1), 0, (iy + i) / (float)(textureSize - 1));	// rest position of the neighbour particle
			 float rest_length = vmath::length(posNeighRest - posRest);	// rest length of the spring

			 force += (dir * (vmath::length(diff) - rest_length)) * stretchStiffness - velocity * damp * 2.5;
			 if (k < 8)
			 {
				 iterator++;
			 }
		 }

		 normal = vmath::normalize(normal / (float)-(iterator - 1));


		 // verlet integration
		 vec3 acc = (force + gravity) * invMass;
		 vec3 tmp = pos;
		 pos = pos * 2 - posOld + acc * particleTimeStep * particleTimeStep;
		 posOld = tmp;

		 // collision with sphere
		 if (collidingObject == COLLIDING_SPHERE)
		 {
			 vec3 center(0.5f, -0.5f, 0.25f);
			 float radius = 0.3f;
			 if (vmath::length(pos - center) < radius)
			 {
				 // collision
				 vec3 dir_coll = vmath::normalize(pos - center);
				 pos = center + dir_coll * radius;
			 }
		 }

		 // collision with plane
		 if (collidingObject == COLLIDING_PLANE)
		 {
			 if (pos[1] <= -0.6f)
			 {
				 pos[1] = -0.6f;
				 posOld += (pos - posOld) * 0.03f;
			 }
		 }

		 // write the results back to buffers
		 positionsNext[index * 4] = pos[0];		positionsNext[index * 4 + 1] = pos[1];		positionsNext[index * 4 + 2] = pos[2];
		 positionsOld[index * 4] = posOld[0];	positionsOld[index * 4 + 1] = posOld[1];	positionsOld[index * 4 + 2] = posOld[2];
		 normals[index * 4] = normal[0];			normals[index * 4 + 1] = normal[1];			normals[index * 4 + 2] = normal[2];
	 }

	 // finally, all the new positions have been computed => they can be stored as the current positions
	 for (int i = 0; i < nbParticles; i++)
	 {
		 positions[i * 4] = positionsNext[i * 4];
		 positions[i * 4 + 1] = positionsNext[i * 4 + 1];
		 positions[i * 4 + 2] = positionsNext[i * 4 + 2];
		 positions[i * 4 + 3] = 1.0f;
	 }

	 unsigned int size = textureSize * textureSize * 4 * sizeof(float);

	 glBindVertexArray(vaoFabric);

	 //positions
	 glBindBuffer(GL_ARRAY_BUFFER, vboPositionFabric);
	 //glBufferData(GL_ARRAY_BUFFER, size, (GLvoid*)&(positions[0]), GL_DYNAMIC_DRAW);
	 glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GLfloat), (GLvoid*)&(positions[0]), GL_DYNAMIC_DRAW);
	 glVertexAttribPointer(SHADER_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	 glEnableVertexAttribArray(SHADER_ATTRIBUTE_POSITION);
	 glBindBuffer(GL_ARRAY_BUFFER, 0);

	 //normals
	 glBindBuffer(GL_ARRAY_BUFFER, vboNormalFabric);
	 //glBufferData(GL_ARRAY_BUFFER, size, (GLvoid*)&(normals[0]), GL_DYNAMIC_DRAW);
	 glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), (GLvoid*)&(normals[0]), GL_DYNAMIC_DRAW);
	 glVertexAttribPointer(SHADER_ATTRIBUTE_NORMAL, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	 glEnableVertexAttribArray(SHADER_ATTRIBUTE_NORMAL);
	 glBindBuffer(GL_ARRAY_BUFFER, 0);

	 glBindVertexArray(0);
 }



 void ClothRenderingShader::TimeStep(void)
 {

	 //code
	 static ComputationType oldComputation = computation;
	 static CollidingObjects oldCollidingObject = collidingObject;

	 //reset memory and re-initialize if the platform changed
	 if ((oldComputation != computation) || (oldCollidingObject != collidingObject))
	 {
		 oldComputation = computation;
		 oldCollidingObject = collidingObject;
		 ResetClothParticleSystem();
		 BuildCloth(textureSize, stretchStiffness, damp, inverseMass);
	 }

	 switch (computation)
	 {
	 case CPU:
		 VerletCPU();
		 break;

	 case CUDA:
	 {
		 float4* pos = NULL, * norm = NULL;
		 if (cuda_graphics_pos_resource != NULL)
		 {
			 UploadCuda(&(positions[0]), &(positionsOld[0]), (int)positions.size());

			 // map OpenGL buffer object for writing from CUDA
			 cudaResult = cudaGraphicsMapResources(1, &cuda_graphics_pos_resource, 0);
			 if (cudaResult != cudaSuccess)
			 {
				 PrintLog("Failed: cudaGraphicsMapResources() failed for cuda_graphics_pos_resource with error: %d\n", (int)cudaResult);
				 return;
			 }

			 size_t num_bytes;
			 cudaResult = cudaGraphicsResourceGetMappedPointer((void**)&pos, &num_bytes, cuda_graphics_pos_resource);
			 if (cudaResult != cudaSuccess)
			 {
				 PrintLog("Failed: cudaGraphicsResourceGetMappedPointer() failed for cuda_graphics_pos_resource with error: %d\n", (int)cudaResult);
				 return;
			 }

			 cudaResult = cudaGraphicsMapResources(1, &cuda_graphics_norm_resource, 0);
			 if (cudaResult != cudaSuccess)
			 {
				 PrintLog("Failed: cudaGraphicsMapResources() failed for cuda_graphics_norm_resource with error: %d\n", (int)cudaResult);
				 return;
			 }

			 num_bytes = 0;
			 cudaResult = cudaGraphicsResourceGetMappedPointer((void**)&norm, &num_bytes, cuda_graphics_norm_resource);
			 if (cudaResult != cudaSuccess)
			 {
				 PrintLog("Failed: cudaGraphicsResourceGetMappedPointer() failed for cuda_graphics_norm_resource with error: %d\n", (int)cudaResult);
				 return;
			 }

			 VerletCuda(pos, norm, &(positions[0]), &(positionsOld[0]), (int)positions.size(), textureSize, stretchStiffness, damp, inverseMass, (int)collidingObject);

			 cudaResult = cudaGraphicsUnmapResources(1, &cuda_graphics_pos_resource, 0);
			 if (cudaResult != cudaSuccess)
			 {
				 PrintLog("Failed: cudaGraphicsUnmapResources() failed for cuda_graphics_pos_resource with error: %d\n", (int)cudaResult);
				 return;
			 }

			 cudaResult = cudaGraphicsUnmapResources(1, &cuda_graphics_norm_resource, 0);
			 if (cudaResult != cudaSuccess)
			 {
				 PrintLog("Failed: cudaGraphicsUnmapResources() failed for cuda_graphics_norm_resource with error: %d\n", (int)cudaResult);
				 return;
			 }
		 }
	 }
	 break;

	 }
 }


void  ClothRenderingShader::InitBuffers(void)
{
	//code
	textureSize = nearest_pow((unsigned int)ceil(sqrt((float)positions.size() / 4.0f)));
	int nbParticles = textureSize * textureSize;

	positionsOld.assign(positions.begin(), positions.end());
	normals.assign(nbParticles * 4, 0.0f);

	// texcoords is used by the fabric shader for visualization
	for (int i = 0; i < nbParticles; i++)
	{
		texcoords.push_back(positions[i * 4]);
		texcoords.push_back(positions[i * 4 + 2]);
	}

	// count how many springs will be simulated in this piece of cloth
	nbSprings = 0;
	for (int index = 0; index < nbParticles; index++)
	{
		int ix = index % textureSize;
		int iy = index / textureSize;

		for (int k = 0; k < 12; k++)
		{
			int i, j;
			NextNeigh(k, i, j);

			if (((iy + i) < 0) || ((iy + i) > (textureSize - 1)))
				continue;

			if (((ix + j) < 0) || ((ix + j) > (textureSize - 1)))
				continue;

			nbSprings++;
		}
	}

	// textures must be entirely filled, in case the side of the cloth is not a power of two
	// so, push back as many 0s as needed
	positions.insert(positions.end(), (nbParticles * 4) - positions.size(), 0.0f);
	positionsOld.insert(positionsOld.end(), (nbParticles * 4) - positionsOld.size(), 0.0f);
	normals.insert(normals.end(), (nbParticles * 4) - normals.size(), 0.0f);
}

void ClothRenderingShader ::ResetClothParticleSystem(void)
{
	//code
	faces.clear();

	positions.clear();
	positionsOld.clear();
	normals.clear();

	texcoords.clear();

	doesInitializationDone = FALSE;
}

void ClothRenderingShader:: ResetClothMemoryAndBuffers(void)
{
	//code
	//delete cuda data structures
	DeleteCudaVBO();
	ResetCuda();
	ResetBuffers();
}
void  ClothRenderingShader ::DeleteCudaVBO(void)
{
	//code
	if (cuda_graphics_pos_resource != NULL)
	{
		// unregister this buffer object with CUDA, if needed
		cudaResult = cudaGraphicsUnregisterResource(cuda_graphics_pos_resource);
		if (cudaResult != cudaSuccess)
		{
			PrintLog("Fail: cudaGraphicsUnregisterResource() failed for cuda_graphics_pos_resource with error: %d\n", (int)cudaResult);
			cuda_graphics_pos_resource = NULL;
			return;
		}

		cuda_graphics_pos_resource = NULL;
	}

	if (cuda_graphics_norm_resource != NULL)
	{
		// unregister this buffer object with CUDA, if needed
		cudaResult = cudaGraphicsUnregisterResource(cuda_graphics_norm_resource);
		if (cudaResult != cudaSuccess)
		{
			PrintLog("Fail: cudaGraphicsUnregisterResource() failed for cuda_graphics_norm_resource with error: %d\n", (int)cudaResult);
			cuda_graphics_norm_resource = NULL;
			return;
		}

		cuda_graphics_norm_resource = NULL;
	}
}
void ClothRenderingShader :: ResetBuffers(void)
{
	if (vaoFabric)
	{
		glDeleteVertexArrays(1, &vaoFabric);
		vaoFabric = 0;
	}

	if (vboPositionFabric)
	{
		glDeleteBuffers(1, &vboPositionFabric);
		vboPositionFabric = 0;
	}

	//destroy vboNormal
	if (vboNormalFabric)
	{
		glDeleteBuffers(1, &vboNormalFabric);
		vboNormalFabric = 0;
	}

	//destroy vboElement
	if (vboFabricElements)
	{
		glDeleteBuffers(1, &vboFabricElements);
		vboFabricElements = 0;
	}
}
void  ClothRenderingShader::NextNeigh(int n, int& i, int& j)
{
	if (n == 0)
	{
		j = -1;
		i = -1;
	}
	if (n == 1)
	{
		j = 0;
		i = -1;
	}
	if (n == 2)
	{
		j = 1;
		i = -1;
	}

	if (n == 3)
	{
		j = 1;
		i = 0;
	}
	if (n == 4)
	{
		j = 1;
		i = 1;
	}
	if (n == 5)
	{
		j = 0;
		i = 1;
	}

	if (n == 6)
	{
		j = -1;
		i = 1;
	}
	if (n == 7)
	{
		j = -1;
		i = 0;
	}

	if (n == 8)
	{
		j = -2;
		i = -2;
	}
	if (n == 9)
	{
		j = 2;
		i = -2;
	}
	if (n == 10)
	{
		j = 2;
		i = 2;
	}
	if (n == 11)
	{
		j = -2;
		i = 2;
	}
}
