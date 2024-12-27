#pragma once



#include "common/helper_math.h"
#include "common/helper_cuda.h"

#include <assert.h>

//#define USE_SMEM
#define BLOCKSIZE (32 * 32)

float4* pCudaPos0 = NULL; float4* pCudaPos1 = NULL;
float4* pCudaPosOld0 = NULL; float4* pCudaPosOld1 = NULL;

float4* pPosIn, * pPosOut;
float4* pPosOldIn, * pPosOldOut;

int iteration = 0;

__device__ int2 NextNeigh(int n)
{
	if (n == 0)
	{
		return make_int2(-1, -1);
	}
	if (n == 1)
	{
		return make_int2(0, -1);
	}
	if (n == 2)
	{
		return make_int2(1, -1);
	}
	if (n == 3)
	{
		return make_int2(1, 0);
	}
	if (n == 4)
	{
		return make_int2(1, 1);
	}
	if (n == 5)
	{
		return make_int2(0, 1);
	}
	if (n == 6)
	{
		return make_int2(-1, 1);
	}
	if (n == 7)
	{
		return make_int2(-1, 0);
	}

	if (n == 8)
	{
		return make_int2(-2, -2);
	}
	if (n == 9)
	{
		return make_int2(2, -2);
	}
	if (n == 10)
	{
		return make_int2(2, 2);
	}
	if (n == 11)
	{
		return make_int2(-2, 2);
	}

	return make_int2(0, 0);
}

__global__ void verlet_cuda(float4* pos_vbo, float4* nor_vbo, float4* g_pos_in, float4* g_pos_old_in, float4* g_pos_out, float4* g_pos_old_out,
	int side, float stiffness, float damp, float inverse_mass, int coll_primitives)
{
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	int ix = index % side;
	int iy = index / side;

	//g_pos[index] = make_float4(threadIdx.x, blockIdx.x, blockDim.x, index);
	//return;

#ifdef USE_SMEM
	__shared__ float4 smem_pos[BLOCKSIZE];
	__shared__ float4 smem_pos_old[BLOCKSIZE];
	__shared__ float4 smem_vel[BLOCKSIZE];

	int ix_smem = threadIdx.x % side;
	int iy_smem = threadIdx.x / side;

	smem_pos[threadIdx.x] = g_pos_in[index];
	smem_pos[threadIdx.x] = g_pos_in[index];
	smem_pos_old[threadIdx.x] = g_pos_old_out[index];

	for (int k = 0; k < 12; k++)
	{
		int2 coord = NextNeigh(k);
		int j = coord.x;
		int i = coord.y;

		if (((iy_smem + i) < 0) || ((iy_smem + i) > (side - 1)))
			continue;

		if (((ix_smem + j) < 0) || ((ix_smem + j) > (side - 1)))
			continue;

		int index_neigh_smem = (iy_smem + i) * side + ix_smem + j;
		int index_neigh = (iy + i) * side + ix + j;

		smem_pos[index_neigh_smem] = g_pos_in[index_neigh];
		smem_pos_old[index_neigh_smem] = g_pos_old_out[index_neigh];
		smem_vel[index_neigh_smem] = g_pos_in[index_neigh];
	}

	__syncthreads();

	volatile float4 posData = smem_pos[threadIdx.x];    // ensure coalesced read
	volatile float4 posOldData = smem_pos_old[threadIdx.x];
	volatile float4 velData = smem_vel[threadIdx.x];
#else
	volatile float4 posData = g_pos_in[index];    // ensure coalesced read
	volatile float4 posOldData = g_pos_old_in[index];
#endif

	float3 pos = make_float3(posData.x, posData.y, posData.z);
	float3 pos_old = make_float3(posOldData.x, posOldData.y, posOldData.z);
	float3 vel = (pos - pos_old) / 0.01;
	float3 gravity = make_float3(0.0, -9.81, 0.0);

	// used for computation of the normal
	float3 normal = make_float3(0, 0, 0);
	float3 last_diff = make_float3(0, 0, 0);
	int iters = 0;

	float3 force = make_float3(0.0, 0.0, 0.0);
	float inv_mass = inverse_mass;
	if (index <= (side - 1.0))
		inv_mass = 0.0f;

	float step = 1.0 / (side - 1.0);

	for (int k = 0; k < 12; k++)
	{
		int2 coord = NextNeigh(k);
		int j = coord.x;
		int i = coord.y;

#ifdef USE_SMEM
		if (((iy_smem + i) < 0) || ((iy_smem + i) > (side - 1)))
			continue;

		if (((ix_smem + j) < 0) || ((ix_smem + j) > (side - 1)))
			continue;

		int index_neigh_smem = (iy_smem + i) * side + ix_smem + j;

		volatile float4 pos_neighData = smem_pos[index_neigh_smem];
#else
		if (((iy + i) < 0) || ((iy + i) > (side - 1)))
			continue;

		if (((ix + j) < 0) || ((ix + j) > (side - 1)))
			continue;

		int index_neigh = (iy + i) * side + ix + j;

		volatile float4 pos_neighData = g_pos_in[index_neigh];
#endif
		float3 pos_neigh = make_float3(pos_neighData.x, pos_neighData.y, pos_neighData.z);

		float3 diff = pos_neigh - pos;

		float3 curr_diff = diff;	// curr diff is the normalized direction of the spring
		curr_diff = normalize(curr_diff);

		if ((iters > 0) && (k < 8))
		{
			float angle = dot(curr_diff, last_diff);
			if (angle > 0.0)
			{
				normal += cross(last_diff, curr_diff);
			}
		}
		last_diff = curr_diff;

		float2 fcoord = make_float2(coord) * step;
		float rest_length = length(fcoord);

		force += (curr_diff * (length(diff) - rest_length)) * stiffness - vel * damp * 0.5;
		if (k < 8)
		{
			iters += 1;
		}
	}

	normal = normalize(normal / -(iters - 1.0));

	float3 acc = make_float3(0, 0, 0);
	acc = (force + gravity) * inv_mass;

	// verlet
	float3 tmp = pos;
	pos = pos * 2 - pos_old + acc * 0.01 * 0.01;
	pos_old = tmp;

	// collision with a sphere
	if (coll_primitives & 1)
	{
		float3 center = make_float3(0.5, -0.5, 0.25);
		float radius = 0.3;

		if (length(pos - center) < radius)
		{
			// collision
			float3 coll_dir = normalize(pos - center);
			pos = center + coll_dir * radius;
		}
	}

	// collision with a plane
	if (coll_primitives & 2)
	{
		if (pos.y < -0.6)
		{
			pos.y = -0.6;
			pos_old += (pos - pos_old) * 0.03;
		}
	}

	__syncthreads();

	pos_vbo[index] = make_float4(pos.x, pos.y, pos.z, posData.w);
	nor_vbo[index] = make_float4(normal.x, normal.y, normal.z, 0.0);

	g_pos_out[index] = make_float4(pos.x, pos.y, pos.z, posData.w);
	g_pos_old_out[index] = make_float4(pos_old.x, pos_old.y, pos_old.z, posOldData.w);
}

// size is the total number of float (= number of particles * 4)
void InitCuda(const int size)
{
	const unsigned int num_threads = size / 4;
	const unsigned int mem_size = sizeof(float4) * num_threads;

	// allocate device memory for float4 version
	checkCudaErrors(cudaMalloc((void**)&pCudaPos0, mem_size));	// positions
	checkCudaErrors(cudaMalloc((void**)&pCudaPos1, mem_size));	// positions
	checkCudaErrors(cudaMalloc((void**)&pCudaPosOld0, mem_size));	// old positions
	checkCudaErrors(cudaMalloc((void**)&pCudaPosOld1, mem_size));	// old positions

	iteration = 0;
}

//Round a / b to nearest higher integer value
int iDivUp(int a, int b)
{
	return (a % b != 0) ? (a / b + 1) : (a / b);
}

// compute grid and thread block size for a given number of elements
void ComputeGridSize(uint n, uint blockSize, uint& numBlocks, uint& numThreads)
{
	numThreads = min(blockSize, n);
	numBlocks = iDivUp(n, numThreads);
}

void UploadCuda(float* positions, float* positions_old, const int size)
{
	assert(pCudaPos0 != NULL);
	assert(pCudaPosOld0 != NULL);

	const unsigned int num_threads = size / 4;
	//cutilCondition(0 == (num_threads % 4));
	const unsigned int mem_size = sizeof(float4) * num_threads;

	// copy host memory to device
	// NOTE: it is not necessary to copy in each iteration, just swap the buffers -> huge save of computation time

	if ((iteration % 2) == 0)
	{
		pPosIn = pCudaPos0;
		pPosOut = pCudaPos1;
		pPosOldIn = pCudaPosOld0;
		pPosOldOut = pCudaPosOld1;
	}
	else
	{
		pPosIn = pCudaPos1;
		pPosOut = pCudaPos0;
		pPosOldIn = pCudaPosOld1;
		pPosOldOut = pCudaPosOld0;
	}

	if (iteration == 0)
	{
		cudaMemcpy(pPosIn, positions, mem_size, cudaMemcpyHostToDevice);
		cudaMemcpy(pPosOldIn, positions_old, mem_size, cudaMemcpyHostToDevice);
	}

	iteration++;
}

void VerletCuda(float4* pos_vbo, float4* nor_vbo, float* positions, float* positions_old, const int size, const int& side, const float& stiffness,
	const float& damp, const float& inverse_mass, const int& coll_primitives)
{
	// setup execution parameters 
	uint numThreads, numBlocks;
	uint numParticles = size / 4;

	ComputeGridSize(numParticles, 256, numBlocks, numThreads);

	// execute the kernel
	//	printf("numParticles: %d,   numThreads: %d   numBlocks: %d\n", numParticles, numThreads, numBlocks);
	verlet_cuda << < numBlocks, numThreads >> > (pos_vbo, nor_vbo, pPosIn, pPosOldIn, pPosOut, pPosOldOut, side, stiffness, damp, inverse_mass, coll_primitives);

	// stop the CPU until the kernel has been executed
	cudaThreadSynchronize();
}

void ResetCuda()
{
	// cleanup memory
	if (pCudaPos0 != NULL)
	{
		cudaFree(pCudaPos0);
		cudaFree(pCudaPos1);
		pCudaPos0 = NULL;
		pCudaPos1 = NULL;
	}

	if (pCudaPosOld0 != NULL)
	{
		cudaFree(pCudaPosOld0);
		cudaFree(pCudaPosOld1);
		pCudaPosOld0 = NULL;
		pCudaPosOld1 = NULL;
	}
}
