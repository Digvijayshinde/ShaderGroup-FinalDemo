#pragma once
#include "../../Utility/Headers/Ocean/OceanHelper.h"
#include "../../Utility/Headers/Ocean/OpenGLMesh.h"
#include "../../Utility/Headers/Ocean/OceanQuadMesh.h"
#include "OceanShader.h"
#include "../../Source/Globals.h"
#include <cassert>

//shader variables
static GLuint updateSpectrumShaderProgramObject;
static GLuint fourierFFTShaderProgramObject;
static GLuint displacementShaderProgramObject;
static GLuint gradientsShaderProgramObject;
static GLuint oceanShaderProgramObject;

//shader uniforms

//UpdateSpectrum
static GLuint updateSpectrumTimeUniform;

//oceanShader
static GLuint displacementUniform;
static GLuint perlinUniform;
static GLuint matLocalUniform;
static GLuint matWorldUniform;
static GLuint matViewProjUniform;
static GLuint uvParamsUniform;
static GLuint perlinOffsetUniform;
static GLuint eyePosUniform;
static GLuint envmapUniform;
static GLuint gradientsUniform;
static GLuint oceanColorUniform;

static GLuint sunPositionUniform;

Math::Color OceanColors[] =
{
	{ 0.0056f, 0.0194f, 0.0331f, 1 },	// deep blue
	{ 0.1812f, 0.4678f, 0.5520f, 1 },	// carribbean
	{ 0.0000f, 0.2307f, 0.3613f, 1 },	// light blue
	{ 0.2122f, 0.6105f, 1.0000f, 1 },
	{ 0.0123f, 0.3613f, 0.6867f, 1 },
	{ 0.0000f, 0.0999f, 0.4508f, 1 },
	{ 0.0000f, 0.0331f, 0.1329f, 1 },
	{ 0.0000f, 0.0103f, 0.0331f, 1 }
};

static GLuint	initial = 0;		// initial spectrum \tilde{h}_0
static GLuint	frequencies = 0;		// frequency \omega_i per wave vector
static GLuint	updated[2] = { 0 };	// updated spectra \tilde{h}(\mathbf{k},t) and \tilde{\mathbf{D}}(\mathbf{k},t) [reused for FT result]
static GLuint	tempdata = 0;		// intermediate data for FT
static GLuint	displacement = 0;		// displacement map
static GLuint	gradients = 0;		// normal & folding map
static GLuint	perlintex = 0;		// Perlin noise to remove tiling artifacts
static GLuint	environment = 0;
static GLuint	debugvao = 0;

uint32_t		numlods = 0;
int				currentcolor = 6;

OpenGLMesh* oceanmesh = nullptr;
OceanQuadMesh		tree;
OceanCamera		camera;

static float yawValue = 30.0f;
static float pitchValue = 1.700001f;
static float eyePos_X = 0.0f;
static float eyePos_Y = 2.0f;
static float eyePos_Z = 0.0f;
static float O_FOVY = 60.0f;
static float fSunPosition = -1.0f;

// forward declarations
extern void FFT_Test();

void FourierTransform(GLuint spectrum);
void GenerateLODLevels(OpenGLAttributeRange** subsettable, GLuint* numsubsets, uint32_t* idata);
GLuint GenerateBoundaryMesh(int deg_left, int deg_top, int deg_right, int deg_bottom, int levelsize, uint32_t* idata);

BOOL cameraSetForHajiAli = FALSE;
BOOL cameraSetForShipsScene = FALSE;

// static functions
static float Phillips(const Math::Vector2& k, const Math::Vector2& w, float V, float A)
{
	float L = (V * V) / GRAV_ACCELERATION;	// largest possible wave for wind speed V
	float l = L / 1000.0f;					// supress waves smaller than this

	float kdotw = Math::Vec2Dot(k, w);
	float k2 = Math::Vec2Dot(k, k);			// squared length of wave vector k

	// k^6 because k must be normalized
	float P_h = A * (expf(-1.0f / (k2 * L * L))) / (k2 * k2 * k2) * (kdotw * kdotw);

	if (kdotw < 0.0f) 
	{
		// wave is moving against wind direction w
		P_h *= 0.07f;
	}

	return P_h * expf(-k2 * l * l);
}

static float Fresnel(float alpha, float n1, float n2)
{
	// air = 1.000293f, water = 1.33f
	float sina = sinf(alpha);
	float sing = sina * (n1 / n2);
	float cosa = cosf(alpha);
	float cosg = sqrtf(1.0f - sing * sing);

	float Rs = (n1 * cosa - n2 * cosg) / (n1 * cosa + n2 * cosg);
	float Rp = (n1 * cosg - n2 * cosa) / (n1 * cosg + n2 * cosa);

	Rs *= Rs;
	Rp *= Rp;

	return 0.5f * (Rs + Rp);
}

static GLuint CalcSubsetIndex(int level, int dL, int dR, int dB, int dT)
{
	// returns the subset index of the given LOD levels
	return 2 * (level * 3 * 3 * 3 * 3 + dL * 3 * 3 * 3 + dR * 3 * 3 + dB * 3 + dT);
}

void FFT_Test()
{
	const int N = 16;

	Math::Complex x[N] = {
		Math::Complex(1, 0),
		Math::Complex(2, -1),
		Math::Complex(0, -1),
		Math::Complex(-1, 2),
		Math::Complex(3, 0),
		Math::Complex(-2, 4),
		Math::Complex(1, -3),
		Math::Complex(0, 2),
		Math::Complex(1, 2),
		Math::Complex(-2, -1),
		Math::Complex(5, -1),
		Math::Complex(3, 4),
		Math::Complex(4, -2),
		Math::Complex(-4, 3),
		Math::Complex(1, 0),
		Math::Complex(-2, 5)
	};

	Math::Complex X_dft[N];
	Math::Complex X_fft[N];

	// DFT
	for (int k = 0; k < N; ++k) {
		X_dft[k] = Math::Complex(0, 0);

		for (int n = 0; n < N; ++n) {
			float theta = (Math::TWO_PI * k * n) / N;
			X_dft[k] += x[n] * Math::Complex(cosf(theta), -sinf(theta));
		}
	}

	// FFT
	int log2_N = Math::Log2OfPow2(N);

	for (int j = 0; j < N; ++j) {
		int nj = (Math::ReverseBits32(j) >> (32 - log2_N)) & (N - 1);
		X_fft[nj] = x[j];
	}

	// modified iterative algorithm for compute shader
	Math::Complex T[N];
	Math::Complex(*pingpong[2])[N] = { &X_fft, &T };
	int src = 0;

	for (int s = 1; s <= log2_N; ++s) {
		int m = 1 << s;				// butterfly group height
		int mh = m >> 1;			// butterfly group half height

		// includes redundant butterfly calculations
		for (int l = 0; l < N; ++l) {
			int k = (l * (N / m)) & (N - 1);
			int i = (l & ~(m - 1));	// butterfly group starting offset
			int j = (l & (mh - 1));	// butterfly index in group

			// twiddle factor W_N^k
			float theta = (Math::TWO_PI * k) / (float)N;
			Math::Complex W_N_k(cosf(theta), -sinf(theta));

			Math::Complex t = W_N_k * (*pingpong[src])[i + j + mh];
			Math::Complex u = (*pingpong[src])[i + j];

			(*pingpong[1 - src])[l] = u + t;
		}

		src = 1 - src;
	}

	// test
	char buff[64];

	PrintLog("       DFT       vs       FFT       \n------------------------------------\n");

	for (int k = 0; k < N; ++k) 
	{
		sprintf_s(buff, "(%.3f, %.3f)", X_dft[k].a, X_dft[k].b);

		size_t len = strlen(buff);
		size_t start = 19;

		_strnset_s(buff, ' ', start - len);
		buff[start - len] = 0;

		PrintLog("(%.3f, %.3f)%s(%.3f, %.3f)\n", X_dft[k].a, X_dft[k].b, buff, X_fft[k].a, X_fft[k].b);
	}
}

bool GLCreateMesh(GLuint numvertices, GLuint numindices, GLuint options, OpenGLVertexElement* decl, OpenGLMesh** mesh)
{
	OpenGLMesh* glmesh = new OpenGLMesh();
	int numdeclelems = 0;

	for (int i = 0; i < 16; ++i) {
		++numdeclelems;

		if (decl[i].Stream == 0xff)
			break;
	}

	glGenBuffers(1, &glmesh->vertexbuffer);

	if (numvertices >= 0xffff)
		options |= GLMESH_32BIT;

	glmesh->meshoptions = options;
	glmesh->numsubsets = 1;
	glmesh->numvertices = numvertices;
	glmesh->numindices = numindices;
	glmesh->subsettable = new OpenGLAttributeRange[1];

	glmesh->subsettable->AttribId = 0;
	glmesh->subsettable->IndexCount = numindices;
	glmesh->subsettable->IndexStart = 0;
	glmesh->subsettable->PrimitiveType = GL_TRIANGLES;
	glmesh->subsettable->VertexCount = (numindices > 0 ? 0 : numvertices);
	glmesh->subsettable->VertexStart = 0;
	glmesh->subsettable->Enabled = GL_TRUE;

	glmesh->vertexdecl.Elements = new OpenGLVertexElement[numdeclelems];
	memcpy(glmesh->vertexdecl.Elements, decl, numdeclelems * sizeof(OpenGLVertexElement));

	// create vertex layout
	glmesh->RecreateVertexLayout();
	GL_ASSERT(glmesh->vertexdecl.Stride != 0);

	// allocate storage
	GLenum usage = ((options & GLMESH_DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	GLuint istride = ((options & GLMESH_32BIT) ? 4 : 2);

	glBindBuffer(GL_ARRAY_BUFFER, glmesh->vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, numvertices * glmesh->vertexdecl.Stride, 0, usage);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (numindices > 0) {
		glGenBuffers(1, &glmesh->indexbuffer);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glmesh->indexbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numindices * istride, 0, usage);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	(*mesh) = glmesh;
	return true;
}

int initializeOceanShader(void)
{
	//code
	std::mt19937 gen;
	std::normal_distribution<> gaussian(0.0, 1.0);
	GLint maxanisotropy = 1;

	// test the modified FFT algorithm
	//FFT_Test();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);

	glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxanisotropy);
	maxanisotropy = Math::Max(maxanisotropy, 2);

	// generate initial spectrum and frequencies
	Math::Vector2 k;
	float L = PATCH_SIZE;

	glGenTextures(1, &initial);
	glGenTextures(1, &frequencies);

	glBindTexture(GL_TEXTURE_2D, initial);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, DISP_MAP_SIZE + 1, DISP_MAP_SIZE + 1);

	glBindTexture(GL_TEXTURE_2D, frequencies);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, DISP_MAP_SIZE + 1, DISP_MAP_SIZE + 1);

	// n, m should be be in [-N / 2, N / 2]
	int start = DISP_MAP_SIZE / 2;

	// NOTE: in order to be symmetric, this must be (N + 1) x (N + 1) in size
	Math::Complex* h0data = new Math::Complex[(DISP_MAP_SIZE + 1) * (DISP_MAP_SIZE + 1)];
	float* wdata = new float[(DISP_MAP_SIZE + 1) * (DISP_MAP_SIZE + 1)];
	{
		Math::Vector2 w = WIND_DIRECTION;
		Math::Vector2 wn;
		float V = WIND_SPEED;
		float A = AMPLITUDE_CONSTANT;

		Math::Vec2Normalize(wn, w);

		for (int m = 0; m <= DISP_MAP_SIZE; ++m)
		{
			k.y = (Math::TWO_PI * (start - m)) / L;

			for (int n = 0; n <= DISP_MAP_SIZE; ++n)
			{
				k.x = (Math::TWO_PI * (start - n)) / L;

				int index = m * (DISP_MAP_SIZE + 1) + n;
				float sqrt_P_h = 0;

				if (k.x != 0.0f || k.y != 0.0f)
					sqrt_P_h = sqrtf(Phillips(k, wn, V, A));

				h0data[index].a = (float)(sqrt_P_h * gaussian(gen) * Math::ONE_OVER_SQRT_2);
				h0data[index].b = (float)(sqrt_P_h * gaussian(gen) * Math::ONE_OVER_SQRT_2);

				// dispersion relation \omega^2(k) = gk
				wdata[index] = sqrtf(GRAV_ACCELERATION * Math::Vec2Length(k));
			}
		}
	}

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, DISP_MAP_SIZE + 1, DISP_MAP_SIZE + 1, GL_RED, GL_FLOAT, wdata);

	glBindTexture(GL_TEXTURE_2D, initial);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, DISP_MAP_SIZE + 1, DISP_MAP_SIZE + 1, GL_RG, GL_FLOAT, h0data);

	delete[] wdata;
	delete[] h0data;

	// create other spectrum textures
	glGenTextures(2, updated);
	glBindTexture(GL_TEXTURE_2D, updated[0]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, DISP_MAP_SIZE, DISP_MAP_SIZE);

	glBindTexture(GL_TEXTURE_2D, updated[1]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, DISP_MAP_SIZE, DISP_MAP_SIZE);

	glGenTextures(1, &tempdata);
	glBindTexture(GL_TEXTURE_2D, tempdata);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, DISP_MAP_SIZE, DISP_MAP_SIZE);

	// create displacement map
	glGenTextures(1, &displacement);
	glBindTexture(GL_TEXTURE_2D, displacement);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, DISP_MAP_SIZE, DISP_MAP_SIZE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// create gradient & folding map
	glGenTextures(1, &gradients);
	glBindTexture(GL_TEXTURE_2D, gradients);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, DISP_MAP_SIZE, DISP_MAP_SIZE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxanisotropy / 2);

	glBindTexture(GL_TEXTURE_2D, 0);

	// create mesh and LOD levels (could use tess shader in the future)
	OpenGLVertexElement decl[] = {
		{ 0, 0, GLDECLTYPE_FLOAT3, GLDECLUSAGE_POSITION, 0 },
		{ 0xff, 0, 0, 0, 0 }
	};

	numlods = Math::Log2OfPow2(MESH_SIZE);

	GLuint numvertices = (MESH_SIZE + 1) * (MESH_SIZE + 1);

	if (!GLCreateMesh(numvertices, IndexCounts[numlods], GLMESH_32BIT, decl, &oceanmesh))
	{
		PrintLog("GLCreateMesh() failed\n");
		return (-1);
	}

	OpenGLAttributeRange* subsettable = nullptr;
	Math::Vector3* vdata = nullptr;
	uint32_t* idata = nullptr;
	GLuint numsubsets = 0;

	oceanmesh->LockVertexBuffer(0, 0, GLLOCK_DISCARD, (void**)&vdata);
	oceanmesh->LockIndexBuffer(0, 0, GLLOCK_DISCARD, (void**)&idata);
	{
		float tilesize = PATCH_SIZE / MESH_SIZE;

		// vertex data
		for (int z = 0; z <= MESH_SIZE; ++z) {
			for (int x = 0; x <= MESH_SIZE; ++x) {
				int index = z * (MESH_SIZE + 1) + x;

				vdata[index].x = (float)x;
				vdata[index].y = (float)z;
				vdata[index].z = 0.0f;
			}
		}

		// index data
		GenerateLODLevels(&subsettable, &numsubsets, idata);
	}
	oceanmesh->UnlockIndexBuffer();
	oceanmesh->UnlockVertexBuffer();

	oceanmesh->SetAttributeTable(subsettable, numsubsets);
	delete[] subsettable;

	if (initializeUpdateSpectrumShader() != 0)
	{
		PrintLog("initializeUpdateSpectrumShader() failed\n");
		return (-1);
	}

	if (initializeFourierFFTShader() != 0)
	{
		PrintLog("initializeFourierFFTShader() failed\n");
		return (-1);
	}

	if (initializeDisplacementShader() != 0)
	{
		PrintLog("initializeDisplacementShader() failed\n");
		return (-1);
	}

	if (initializeGradientsShader() != 0)
	{
		PrintLog("initializeGradientsShader() failed\n");
		return (-1);
	}

	if (initializeOceanEffect() != 0)
	{
		PrintLog("initializeOceanEffect() failed\n");
		return (-1);
	}

	TextureManager *texturemanager = new TextureManager();
	texturemanager->storeTextureFromFile("Media\\Textures\\Ocean", "perlin_noise.dds");
	/*if (! new TextureManager.createTextureFromDDSFile("Textures/perlin_noise.dds", false, perlintex))
	{
		PrintLog("initializeOceanEffect() failed\n");
		return (-1);
	}*/
	perlintex = texturemanager->getTexture("perlin_noise");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxanisotropy / 2);

	/*if (!GLCreateCubeTextureFromDDS("../../Media/Textures/ocean_env.dds", true, &environment))
		return false;*/

	glGenVertexArrays(1, &debugvao);
	glBindVertexArray(debugvao);
	{
		// empty
	}
	glBindVertexArray(0);

	// init quadtree
	float ocean_extent = PATCH_SIZE * (1 << FURTHEST_COVER);
	float ocean_start[2] = { -0.5f * ocean_extent, -0.5f * ocean_extent };

	tree.Initialize(ocean_start, ocean_extent, (int)numlods, MESH_SIZE, PATCH_SIZE, MAX_COVERAGE, (float)(gWinWidth * gWinHeight));

	// setup camera
	camera.SetFov(Math::DegreesToRadians(O_FOVY));
	camera.SetAspect((float)gWinWidth / (float)gWinHeight);
	camera.SetClipPlanes(1.0f, 2000.0f);
	camera.SetSpeed(camera.GetSpeed() * 3.0f);

	camera.SetEyePosition(eyePos_X, eyePos_Y, eyePos_Z);
	camera.SetOrientation(Math::DegreesToRadians(yawValue), Math::DegreesToRadians(pitchValue), 0);

	// reproable VFC bug (see CHOOPY_SCALE_CORRECTION)
	//camera.SetEyePosition(39.043621f, 7.9374733f, -71.800049f);
	//camera.SetOrientation(1.7278752f, 1.5707960f, 0);

	// test position for article
	//camera.SetEyePosition(-0.068656817f, 3.6943157f, 11.333683f);
	//camera.SetOrientation(1.5446160f, 0.49741900f, 0);
	//camera.SetOrientation(-2.7881632, 0.20943955, 0);

	glDisable(GL_CULL_FACE);

	return (0);
}

void setCameraForShipsScene(void)
{
	// setup camera
	camera.SetFov(Math::DegreesToRadians(60.0f));
	camera.SetEyePosition(0.0f, 2.0f, 0.0f);
	camera.SetOrientation(Math::DegreesToRadians(30.0f), Math::DegreesToRadians(1.700001f), 0);
}

void setCameraForHajiAli(void)
{
	/*
		Camera Details :
	Camera Position : -90.00f, 1.80f, -112.20f
		Camera Front : -0.00f, 0.00f, -1.00f
		Camera Yaw : -90.00f
		Camera Pitch : 0.00f
		yawValue : 31.400005
		pitchValue : 2.500000
		eyePos_X = -0.300000f; eyePos_Y = 15.500023f; eyePos_Z = 0.000000f;
	O_FOVY: */

	// setup camera
	camera.SetFov(Math::DegreesToRadians(80.599686f));
	camera.SetAspect((float)gWinWidth / (float)gWinHeight);
	camera.SetClipPlanes(1.0f, 2000.0f);
	camera.SetSpeed(camera.GetSpeed() * 1.0f);

	camera.SetEyePosition(-0.300000f + eyePos_X, 15.500023f + eyePos_Y, 0.000000f + eyePos_Z);
	camera.SetOrientation(Math::DegreesToRadians(31.400005f), Math::DegreesToRadians(2.500000f), 0);
}

void uninitializeOceanShader(void)
{
	//code
	uninitializeUpdateSpectrumShader();
	uninitializeFourierFFTShader();
	uninitializeDisplacementShader();
	uninitializeGradientsShader();
	uninitializeOceanEffect();
}

int initializeUpdateSpectrumShader(void)
{
	//code
	//Initialize compute shader
	GLuint computeShaderObject = glCreateShader(GL_COMPUTE_SHADER);
	const GLchar* computeShaderSourceCode =
		R"""(#version 460 core

		#define DISP_MAP_SIZE	512
		#define LOG2_DISP_MAP_SIZE	9
		#define TILE_SIZE_X2	0.0781
		#define INV_TILE_SIZE	25.6000

		layout (rg32f, binding = 0) uniform readonly image2D tilde_h0;
		layout (r32f, binding = 1) uniform readonly image2D frequencies;

		layout (rg32f, binding = 2) uniform writeonly image2D tilde_h;
		layout (rg32f, binding = 3) uniform writeonly image2D tilde_D;

		uniform float time;

		layout (local_size_x = 16, local_size_y = 16) in;
		void main()
		{
			ivec2 loc1	= ivec2(gl_GlobalInvocationID.xy);
			ivec2 loc2	= ivec2(DISP_MAP_SIZE - loc1.x, DISP_MAP_SIZE - loc1.y);

			vec2 h_tk;
			vec2 h0_k	= imageLoad(tilde_h0, loc1).rg;
			vec2 h0_mk	= imageLoad(tilde_h0, loc2).rg;
			float w_k	= imageLoad(frequencies, loc1).r;

			// Euler's formula: e^{ix} = \cos x + i \sin x
			float cos_wt = cos(w_k * time);
			float sin_wt = sin(w_k * time);

			// heightfield spectrum
			// \tilde{h}(\mathbf{k},t) = \tilde{tilde_h0}(\mathbf{k}) \ e^{i\omega(k)t} + \tilde{tilde_h0^{*}}(-\mathbf{k}) \ e^{-i\omega(k)t}
			h_tk.x = cos_wt * (h0_k.x + h0_mk.x) - sin_wt * (h0_k.y + h0_mk.y);
			h_tk.y = cos_wt * (h0_k.y - h0_mk.y) + sin_wt * (h0_k.x - h0_mk.x);

			// choppy field spectra
			// \tilde{\mathbf{D}}(\mathbf{k},t) = -i \frac{\mathbf{k}}{k} \ \tilde{h}(\mathbf{k},t)
			vec2 k;

			k.x = float(DISP_MAP_SIZE / 2 - loc1.x);
			k.y = float(DISP_MAP_SIZE / 2 - loc1.y);

			float kn2 = dot(k, k);
			vec2 nk = vec2(0.0, 0.0);

			if (kn2 > 1e-12)
				nk = normalize(k);

			// take advantage of DFT's linearity
			vec2 Dt_x = vec2(h_tk.y * nk.x, -h_tk.x * nk.x);
			vec2 iDt_z = vec2(h_tk.x * nk.y, h_tk.y * nk.y);

			// write ouptut
			imageStore(tilde_h, loc1, vec4(h_tk, 0.0, 0.0));
			imageStore(tilde_D, loc1, vec4(Dt_x + iDt_z, 0.0, 0.0));
		})""";

	glShaderSource(computeShaderObject, 1, (const GLchar**)&computeShaderSourceCode, NULL);

	//compile shader
	glCompileShader(computeShaderObject);
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;

	glGetShaderiv(computeShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(computeShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(computeShaderObject, iInfoLogLength, &written, szInfoLog);
				PrintLog("Update Spectrum Compute Shader Compilation log: %s\n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return (-1);
			}
		}
	}

	//create compute shader program
	updateSpectrumShaderProgramObject = glCreateProgram();

	//attach compute shader object
	glAttachShader(updateSpectrumShaderProgramObject, computeShaderObject);

	//pre-link

	//link shader
	glLinkProgram(updateSpectrumShaderProgramObject);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(updateSpectrumShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(updateSpectrumShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(updateSpectrumShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				PrintLog("Update Spectrum Compute shader program link log: %s\n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;

				return (-1);
			}
		}
	}

	updateSpectrumTimeUniform = glGetUniformLocation(updateSpectrumShaderProgramObject, "time");

	return (0);
}

void uninitializeUpdateSpectrumShader(void)
{
	//code
	if (updateSpectrumShaderProgramObject)
	{
		glUseProgram(updateSpectrumShaderProgramObject);

		GLsizei numAttachedShaders;
		glGetProgramiv(updateSpectrumShaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);

		GLuint* shaderObjects = NULL;
		shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));

		glGetAttachedShaders(updateSpectrumShaderProgramObject, numAttachedShaders, &numAttachedShaders, shaderObjects);

		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(updateSpectrumShaderProgramObject, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}

		free(shaderObjects);
		shaderObjects = NULL;

		glUseProgram(0);

		glDeleteProgram(updateSpectrumShaderProgramObject);
		updateSpectrumShaderProgramObject = 0;
	}
}

int initializeFourierFFTShader(void)
{
	//code
	//Initialize compute shader
	GLuint computeShaderObject = glCreateShader(GL_COMPUTE_SHADER);
	const GLchar* computeShaderSourceCode =
		R"""(#version 460 core

		#define PI		3.1415926535897932
		#define TWO_PI	6.2831853071795864
		#define DISP_MAP_SIZE	512
		#define LOG2_DISP_MAP_SIZE	9
		#define TILE_SIZE_X2	0.0781
		#define INV_TILE_SIZE	25.6000

		layout (rg32f, binding = 0) uniform readonly image2D readbuff;
		layout (rg32f, binding = 1) uniform writeonly image2D writebuff;

		vec2 ComplexMul(vec2 z, vec2 w) {
			return vec2(z.x * w.x - z.y * w.y, z.y * w.x + z.x * w.y);
		}

		shared vec2 pingpong[2][DISP_MAP_SIZE];

		layout (local_size_x = DISP_MAP_SIZE) in;
		void main()
		{
			const float N = float(DISP_MAP_SIZE);

			int z = int(gl_WorkGroupID.x);
			int x = int(gl_LocalInvocationID.x);

			// STEP 1: load row/column and reorder
			int nj = (bitfieldReverse(x) >> (32 - LOG2_DISP_MAP_SIZE)) & (DISP_MAP_SIZE - 1);
			pingpong[0][nj] = imageLoad(readbuff, ivec2(z, x)).rg;

			barrier();

			// STEP 2: perform butterfly passes
			int src = 0;

			for (int s = 1; s <= LOG2_DISP_MAP_SIZE; ++s) {
				int m = 1 << s;				// butterfly group height
				int mh = m >> 1;			// butterfly group half height

				int k = (x * (DISP_MAP_SIZE / m)) & (DISP_MAP_SIZE - 1);
				int i = (x & ~(m - 1));		// butterfly group starting offset
				int j = (x & (mh - 1));		// butterfly index in group

				// twiddle factor W_N^k
				float theta = (TWO_PI * float(k)) / N;
				vec2 W_N_k = vec2(cos(theta), sin(theta));

				vec2 input1 = pingpong[src][i + j + mh];
				vec2 input2 = pingpong[src][i + j];

				src = 1 - src;
				pingpong[src][x] = input2 + ComplexMul(W_N_k, input1);

				barrier();
			}

			// STEP 3: write output
			vec2 result = pingpong[src][x];
			imageStore(writebuff, ivec2(x, z), vec4(result, 0.0, 1.0));

			// NOTE: do sign correction later
		})""";

	glShaderSource(computeShaderObject, 1, (const GLchar**)&computeShaderSourceCode, NULL);

	//compile shader
	glCompileShader(computeShaderObject);
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;

	glGetShaderiv(computeShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(computeShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(computeShaderObject, iInfoLogLength, &written, szInfoLog);
				PrintLog("Fourier FFT Compute Shader Compilation log: %s\n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return (-1);
			}
		}
	}

	//create compute shader program
	fourierFFTShaderProgramObject = glCreateProgram();

	//attach compute shader object
	glAttachShader(fourierFFTShaderProgramObject, computeShaderObject);

	//pre-link

	//link shader
	glLinkProgram(fourierFFTShaderProgramObject);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(fourierFFTShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(fourierFFTShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(fourierFFTShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				PrintLog("Fourier FFT Compute shader program link log: %s\n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;

				return (-1);
			}
		}
	}

	return (0);
}

void uninitializeFourierFFTShader(void)
{
	//code
	if (fourierFFTShaderProgramObject)
	{
		glUseProgram(fourierFFTShaderProgramObject);

		GLsizei numAttachedShaders;
		glGetProgramiv(fourierFFTShaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);

		GLuint* shaderObjects = NULL;
		shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));

		glGetAttachedShaders(fourierFFTShaderProgramObject, numAttachedShaders, &numAttachedShaders, shaderObjects);

		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(fourierFFTShaderProgramObject, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}

		free(shaderObjects);
		shaderObjects = NULL;

		glUseProgram(0);

		glDeleteProgram(fourierFFTShaderProgramObject);
		fourierFFTShaderProgramObject = 0;
	}
}

int initializeDisplacementShader(void)
{
	//code
	//Initialize compute shader
	GLuint computeShaderObject = glCreateShader(GL_COMPUTE_SHADER);
	const GLchar* computeShaderSourceCode =
		R"""(#version 460 core

		layout (rg32f, binding = 0) uniform readonly image2D heightfield;
		layout (rg32f, binding = 1) uniform readonly image2D choppyfield;

		layout (rgba32f, binding = 2) uniform writeonly image2D displacement;

		layout (local_size_x = 16, local_size_y = 16) in;
		void main()
		{
			const float lambda = 1.3f;

			ivec2 loc = ivec2(gl_GlobalInvocationID.xy);

			// required due to interval change
			float sign_correction = ((((loc.x + loc.y) & 1) == 1) ? -1.0 : 1.0);

			float h = sign_correction * imageLoad(heightfield, loc).x;
			vec2 D = sign_correction * imageLoad(choppyfield, loc).xy;

			imageStore(displacement, loc, vec4(D.x * lambda, h, D.y * lambda, 1.0));
		})""";

	glShaderSource(computeShaderObject, 1, (const GLchar**)&computeShaderSourceCode, NULL);

	//compile shader
	glCompileShader(computeShaderObject);
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;

	glGetShaderiv(computeShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(computeShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(computeShaderObject, iInfoLogLength, &written, szInfoLog);
				PrintLog("Displacement Compute Shader Compilation log: %s\n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return (-1);
			}
		}
	}

	//create compute shader program
	displacementShaderProgramObject = glCreateProgram();

	//attach compute shader object
	glAttachShader(displacementShaderProgramObject, computeShaderObject);

	//pre-link

	//link shader
	glLinkProgram(displacementShaderProgramObject);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(displacementShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(displacementShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(displacementShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				PrintLog("Displacement Compute shader program link log: %s\n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return (-1);
			}
		}
	}

	return (0);
}

void uninitializeDisplacementShader(void)
{
	//code
	if (displacementShaderProgramObject)
	{
		glUseProgram(displacementShaderProgramObject);

		GLsizei numAttachedShaders;
		glGetProgramiv(displacementShaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);

		GLuint* shaderObjects = NULL;
		shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));

		glGetAttachedShaders(displacementShaderProgramObject, numAttachedShaders, &numAttachedShaders, shaderObjects);

		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(displacementShaderProgramObject, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}

		free(shaderObjects);
		shaderObjects = NULL;

		glUseProgram(0);

		glDeleteProgram(displacementShaderProgramObject);
		displacementShaderProgramObject = 0;
	}
}

int initializeGradientsShader(void)
{
	//code
	//Initialize compute shader
	GLuint computeShaderObject = glCreateShader(GL_COMPUTE_SHADER);
	const GLchar* computeShaderSourceCode =
		R"""(#version 460 core

		#define DISP_MAP_SIZE	512
		#define LOG2_DISP_MAP_SIZE	9
		#define TILE_SIZE_X2	0.0781
		#define INV_TILE_SIZE	25.6000

		layout (rgba32f, binding = 0) uniform readonly image2D displacement;
		layout (rgba16f, binding = 1) uniform writeonly image2D gradients;

		layout (local_size_x = 16, local_size_y = 16) in;
		void main()
		{
			ivec2 loc = ivec2(gl_GlobalInvocationID.xy);

			// gradient
			ivec2 left			= (loc - ivec2(1, 0)) & (DISP_MAP_SIZE - 1);
			ivec2 right			= (loc + ivec2(1, 0)) & (DISP_MAP_SIZE - 1);
			ivec2 bottom		= (loc - ivec2(0, 1)) & (DISP_MAP_SIZE - 1);
			ivec2 top			= (loc + ivec2(0, 1)) & (DISP_MAP_SIZE - 1);

			vec3 disp_left		= imageLoad(displacement, left).xyz;
			vec3 disp_right		= imageLoad(displacement, right).xyz;
			vec3 disp_bottom	= imageLoad(displacement, bottom).xyz;
			vec3 disp_top		= imageLoad(displacement, top).xyz;

			vec2 gradient		= vec2(disp_left.y - disp_right.y, disp_bottom.y - disp_top.y);

			// Jacobian
			vec2 dDx = (disp_right.xz - disp_left.xz) * INV_TILE_SIZE;
			vec2 dDy = (disp_top.xz - disp_bottom.xz) * INV_TILE_SIZE;

			float J = (1.0 + dDx.x) * (1.0 + dDy.y) - dDx.y * dDy.x;

			// NOTE: normals are in tangent space for now
			imageStore(gradients, loc, vec4(gradient, TILE_SIZE_X2, J));
		})""";

	glShaderSource(computeShaderObject, 1, (const GLchar**)&computeShaderSourceCode, NULL);

	//compile shader
	glCompileShader(computeShaderObject);
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;

	glGetShaderiv(computeShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(computeShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(computeShaderObject, iInfoLogLength, &written, szInfoLog);
				PrintLog("Gradients Compute Shader Compilation log: %s\n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return (-1);
			}
		}
	}

	//create compute shader program
	gradientsShaderProgramObject = glCreateProgram();

	//attach compute shader object
	glAttachShader(gradientsShaderProgramObject, computeShaderObject);

	//pre-link

	//link shader
	glLinkProgram(gradientsShaderProgramObject);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gradientsShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gradientsShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gradientsShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				PrintLog("Gradients Compute shader program link log: %s\n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				return (-1);
			}
		}
	}

	return (0);
}

void uninitializeGradientsShader(void)
{
	//code
	if (gradientsShaderProgramObject)
	{
		glUseProgram(gradientsShaderProgramObject);

		GLsizei numAttachedShaders;
		glGetProgramiv(gradientsShaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);

		GLuint* shaderObjects = NULL;
		shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));

		glGetAttachedShaders(gradientsShaderProgramObject, numAttachedShaders, &numAttachedShaders, shaderObjects);

		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(gradientsShaderProgramObject, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}

		free(shaderObjects);
		shaderObjects = NULL;

		glUseProgram(0);

		glDeleteProgram(gradientsShaderProgramObject);
		gradientsShaderProgramObject = 0;
	}
}


int initializeOceanEffect(void)
{
	//code
	//Vertex shader
	const GLchar* vertexShaderSourceCode =
		R""""(#version 460 core
		// NOTE: also defined in fragment shader
		#define BLEND_START		8		// m
		#define BLEND_END		200		// m

		in vec3 my_Position;

		layout (binding = 0) uniform sampler2D displacement;
		layout (binding = 1) uniform sampler2D perlin;

		uniform mat4 matLocal;
		uniform mat4 matWorld;
		uniform mat4 matViewProj;
		uniform vec4 uvParams;
		uniform vec2 perlinOffset;
		uniform vec3 eyePos;

		out vec3 vdir;
		out vec2 tex;

		void main()
		{
			// NOTE: also defined in fragment shader
			const vec3 perlinFrequency	= vec3(1.12, 0.59, 0.23);
			const vec3 perlinAmplitude	= vec3(0.35, 0.42, 0.57);

			// transform to world space
			vec4 pos_local = matLocal * vec4(my_Position, 1.0);
			vec2 uv_local = pos_local.xy * uvParams.x + vec2(uvParams.y);
			vec3 disp = texture(displacement, uv_local).xyz;

			pos_local = matWorld * pos_local;
			vdir = eyePos - pos_local.xyz;
			tex = uv_local;

			// blend with Perlin waves
			float dist = length(vdir.xz);
			float factor = clamp((BLEND_END - dist) / (BLEND_END - BLEND_START), 0.0, 1.0);
			float perl = 0.0;

			if (factor < 1.0) {
				vec2 ptex = uv_local + uvParams.zw;
		
				float p0 = texture(perlin, ptex * perlinFrequency.x + perlinOffset).a;
				float p1 = texture(perlin, ptex * perlinFrequency.y + perlinOffset).a;
				float p2 = texture(perlin, ptex * perlinFrequency.z + perlinOffset).a;

				perl = dot(vec3(p0, p1, p2), perlinAmplitude);
			}

			disp = mix(vec3(0.0, perl, 0.0), disp, factor);
			gl_Position = matViewProj * vec4(pos_local.xyz + disp, 1.0);
		})"""";

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const char**)&vertexShaderSourceCode, NULL);

	//compile shader
	glCompileShader(vertexShaderObject);

	GLint status;
	GLint infoLogLength;
	char* log = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, log);

				PrintLog("Ocean Vertex Shader compilation log: %s\n", log);
				free(log);
				log = NULL;
				return (-1);
			}
		}
	}

	//Fragment shader
	const GLchar* fragmentShaderSourceCode =
		R""""(#version 460 core

		#define ONE_OVER_4PI	0.0795774715459476

		// NOTE: also defined in vertex shader
		#define BLEND_START		8		// m
		#define BLEND_END		200		// m

		out vec4 my_FragColor0;

		layout (binding = 1) uniform sampler2D perlin;
		layout (binding = 2) uniform samplerCube envmap;
		layout (binding = 3) uniform sampler2D gradients;

		uniform vec4 uvParams;
		uniform vec2 perlinOffset;
		uniform vec3 oceanColor;
		uniform float sunPositionUniform;

		in vec3 vdir;
		in vec2 tex;

		void main()
		{
			// NOTE: also defined in vertex shader
			const vec3 sunColor			= vec3(1.0, 1.0, 0.47);
			const vec3 perlinFrequency	= vec3(1.12, 0.59, 0.23);
			const vec3 perlinGradient	= vec3(0.014, 0.016, 0.022);
			//const vec3 sundir			= vec3(0.503, 0.240, -0.761);
			const vec3 sundir			= vec3(0.503, 0.240, sunPositionUniform);

			// blend with Perlin waves
			float dist = length(vdir.xz);
			float factor = (BLEND_END - dist) / (BLEND_END - BLEND_START);
			vec2 perl = vec2(0.0);

			factor = clamp(factor * factor * factor, 0.0, 1.0);

			if (factor < 1.0) {
				vec2 ptex = tex + uvParams.zw;

				vec2 p0 = texture(perlin, ptex * perlinFrequency.x + perlinOffset).rg;
				vec2 p1 = texture(perlin, ptex * perlinFrequency.y + perlinOffset).rg;
				vec2 p2 = texture(perlin, ptex * perlinFrequency.z + perlinOffset).rg;

				perl = (p0 * perlinGradient.x + p1 * perlinGradient.y + p2 * perlinGradient.z);
			}

			// calculate thingies
			vec4 grad = texture(gradients, tex);
			grad.xy = mix(perl, grad.xy, factor);

			vec3 n = normalize(grad.xzy);
			vec3 v = normalize(vdir);
			vec3 l = reflect(-v, n);

			float F0 = 0.020018673;
			float F = F0 + (1.0 - F0) * pow(1.0 - dot(n, l), 5.0);

			vec3 refl = texture(envmap, l).rgb;

			// tweaked from ARM/Mali's sample
			float turbulence = max(1.6 - grad.w, 0.0);
			float color_mod = 1.0 + 3.0 * smoothstep(1.2, 1.8, turbulence);

			color_mod = mix(1.0, color_mod, factor);

		#if 1
			// some additional specular (Ward model)
			const float rho = 0.3;
			const float ax = 0.2;
			const float ay = 0.1;

			vec3 h = sundir + v;
			vec3 x = cross(sundir, n);
			vec3 y = cross(x, n);

			float mult = (ONE_OVER_4PI * rho / (ax * ay * sqrt(max(1e-5, dot(sundir, n) * dot(v, n)))));
			float hdotx = dot(h, x) / ax;
			float hdoty = dot(h, y) / ay;
			float hdotn = dot(h, n);

			float spec = mult * exp(-((hdotx * hdotx) + (hdoty * hdoty)) / (hdotn * hdotn));
		#else
			// modified Blinn-Phong model (cheaper)
			float spec = pow(clamp(dot(sundir, l), 0.0, 1.0), 400.0);
		#endif

			my_FragColor0 = vec4(mix(oceanColor, refl * color_mod, F) + sunColor * spec, 1.0);
		})"""";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const char**)&fragmentShaderSourceCode, NULL);

	//compile shader
	glCompileShader(fragmentShaderObject);

	status = 0;;
	infoLogLength = 0;;
	log = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, log);

				PrintLog("Ocean Fragment Shader compilation log: %s\n", log);
				free(log);
				log = NULL;
				return (-1);
			}
		}
	}

	//Shader program object
	oceanShaderProgramObject = glCreateProgram();
	glAttachShader(oceanShaderProgramObject, vertexShaderObject);
	glAttachShader(oceanShaderProgramObject, fragmentShaderObject);

	//pre-link binding
	glBindAttribLocation(oceanShaderProgramObject, SHADER_ATTRIBUTE_POSITION, "my_Position");
	
	glLinkProgram(oceanShaderProgramObject);

	glGetProgramiv(oceanShaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(oceanShaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(oceanShaderProgramObject, infoLogLength, &written, log);

				PrintLog("Ocean Shader program linking log: %s\n", log);
				free(log);
				log = NULL;
				return (-1);
			}
		}
	}

	displacementUniform = glGetUniformLocation(oceanShaderProgramObject, "displacement");
	perlinUniform = glGetUniformLocation(oceanShaderProgramObject, "perlin");
	matLocalUniform = glGetUniformLocation(oceanShaderProgramObject, "matLocal");
	matWorldUniform = glGetUniformLocation(oceanShaderProgramObject, "matWorld");
	matViewProjUniform = glGetUniformLocation(oceanShaderProgramObject, "matViewProj");
	uvParamsUniform = glGetUniformLocation(oceanShaderProgramObject, "uvParams");
	perlinOffsetUniform = glGetUniformLocation(oceanShaderProgramObject, "perlinOffset");
	eyePosUniform = glGetUniformLocation(oceanShaderProgramObject, "eyePos");
	envmapUniform = glGetUniformLocation(oceanShaderProgramObject, "envmap");
	gradientsUniform = glGetUniformLocation(oceanShaderProgramObject, "gradients");
	oceanColorUniform = glGetUniformLocation(oceanShaderProgramObject, "oceanColor");
	sunPositionUniform = glGetUniformLocation(oceanShaderProgramObject, "sunPositionUniform");

	return (0);
}

void uninitializeOceanEffect(void)
{
	//code
	if (oceanShaderProgramObject)
	{
		glUseProgram(oceanShaderProgramObject);

		GLsizei numAttachedShaders;
		glGetProgramiv(oceanShaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);

		GLuint* shaderObjects = NULL;
		shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));

		glGetAttachedShaders(oceanShaderProgramObject, numAttachedShaders, &numAttachedShaders, shaderObjects);

		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(oceanShaderProgramObject, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}

		free(shaderObjects);
		shaderObjects = NULL;

		glUseProgram(0);

		glDeleteProgram(oceanShaderProgramObject);
		oceanShaderProgramObject = 0;
	}
}

void GenerateLODLevels(OpenGLAttributeRange** subsettable, GLuint* numsubsets, uint32_t* idata)
{
#define CALC_INNER_INDEX(x, z) \
	((top + (z)) * (MESH_SIZE + 1) + left + (x))
	// END

	assert(subsettable);
	assert(numsubsets);

	*numsubsets = (numlods - 2) * 3 * 3 * 3 * 3 * 2;
	*subsettable = new OpenGLAttributeRange[*numsubsets];

	int currsubset = 0;
	GLuint indexoffset = 0;
	GLuint numwritten = 0;
	OpenGLAttributeRange* subset = 0;

	//int numrestarts = 0;

	for (uint32_t level = 0; level < numlods - 2; ++level) {
		int levelsize = MESH_SIZE >> level;
		int mindegree = levelsize >> 3;

		for (int left_degree = levelsize; left_degree > mindegree; left_degree >>= 1) {
			for (int right_degree = levelsize; right_degree > mindegree; right_degree >>= 1) {
				for (int bottom_degree = levelsize; bottom_degree > mindegree; bottom_degree >>= 1) {
					for (int top_degree = levelsize; top_degree > mindegree; top_degree >>= 1) {
						int right = ((right_degree == levelsize) ? levelsize : levelsize - 1);
						int left = ((left_degree == levelsize) ? 0 : 1);
						int bottom = ((bottom_degree == levelsize) ? levelsize : levelsize - 1);
						int top = ((top_degree == levelsize) ? 0 : 1);

						// generate inner mesh (triangle strip)
						int width = right - left;
						int height = bottom - top;

						numwritten = 0;

						for (int z = 0; z < height; ++z) {
							if ((z & 1) == 1) {
								idata[numwritten++] = CALC_INNER_INDEX(0, z);
								idata[numwritten++] = CALC_INNER_INDEX(0, z + 1);

								for (int x = 0; x < width; ++x) {
									idata[numwritten++] = CALC_INNER_INDEX(x + 1, z);
									idata[numwritten++] = CALC_INNER_INDEX(x + 1, z + 1);
								}

								idata[numwritten++] = UINT32_MAX;
								//++numrestarts;
							}
							else {
								idata[numwritten++] = CALC_INNER_INDEX(width, z + 1);
								idata[numwritten++] = CALC_INNER_INDEX(width, z);

								for (int x = width - 1; x >= 0; --x) {
									idata[numwritten++] = CALC_INNER_INDEX(x, z + 1);
									idata[numwritten++] = CALC_INNER_INDEX(x, z);
								}

								idata[numwritten++] = UINT32_MAX;
								//++numrestarts;
							}
						}

						// add inner subset
						subset = ((*subsettable) + currsubset);

						subset->AttribId = currsubset;
						subset->Enabled = (numwritten > 0);
						subset->IndexCount = numwritten;
						subset->IndexStart = indexoffset;
						subset->PrimitiveType = GL_TRIANGLE_STRIP;
						subset->VertexCount = 0;
						subset->VertexStart = 0;

						indexoffset += numwritten;
						idata += numwritten;

						++currsubset;

						// generate boundary mesh (triangle list)
						numwritten = GenerateBoundaryMesh(left_degree, top_degree, right_degree, bottom_degree, levelsize, idata);

						// add boundary subset
						subset = ((*subsettable) + currsubset);

						subset->AttribId = currsubset;
						subset->Enabled = (numwritten > 0);
						subset->IndexCount = numwritten;
						subset->IndexStart = indexoffset;
						subset->PrimitiveType = GL_TRIANGLES;
						subset->VertexCount = 0;
						subset->VertexStart = 0;

						indexoffset += numwritten;
						idata += numwritten;

						++currsubset;
					}
				}
			}
		}
	}

	//OpenGLAttributeRange& lastsubset = (*subsettable)[currsubset - 1];
	//PrintLog("Total indices: %lu (%lu restarts)\n", lastsubset.IndexStart + lastsubset.IndexCount, numrestarts);
}

GLuint GenerateBoundaryMesh(int deg_left, int deg_top, int deg_right, int deg_bottom, int levelsize, uint32_t* idata)
{
#define CALC_BOUNDARY_INDEX(x, z) \
	((z) * (MESH_SIZE + 1) + (x))
	// END

	GLuint numwritten = 0;

	// top edge
	if (deg_top < levelsize) {
		int t_step = levelsize / deg_top;

		for (int i = 0; i < levelsize; i += t_step) {
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i, 0);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i + t_step / 2, 1);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i + t_step, 0);

			for (int j = 0; j < t_step / 2; ++j) {
				if (i == 0 && j == 0 && deg_left < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(i, 0);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j, 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j + 1, 1);
			}

			for (int j = t_step / 2; j < t_step; ++j) {
				if (i == levelsize - t_step && j == t_step - 1 && deg_right < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + t_step, 0);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j, 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j + 1, 1);
			}
		}
	}

	// left edge
	if (deg_left < levelsize) {
		int l_step = levelsize / deg_left;

		for (int i = 0; i < levelsize; i += l_step) {
			idata[numwritten++] = CALC_BOUNDARY_INDEX(0, i);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(0, i + l_step);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(1, i + l_step / 2);

			for (int j = 0; j < l_step / 2; ++j) {
				if (i == 0 && j == 0 && deg_top < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(0, i);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(1, i + j + 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(1, i + j);
			}

			for (int j = l_step / 2; j < l_step; ++j) {
				if (i == levelsize - l_step && j == l_step - 1 && deg_bottom < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(0, i + l_step);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(1, i + j + 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(1, i + j);
			}
		}
	}

	// right edge
	if (deg_right < levelsize) {
		int r_step = levelsize / deg_right;

		for (int i = 0; i < levelsize; i += r_step) {
			idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize, i);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize - 1, i + r_step / 2);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize, i + r_step);

			for (int j = 0; j < r_step / 2; ++j) {
				if (i == 0 && j == 0 && deg_top < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize, i);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize - 1, i + j);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize - 1, i + j + 1);
			}

			for (int j = r_step / 2; j < r_step; ++j) {
				if (i == levelsize - r_step && j == r_step - 1 && deg_bottom < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize, i + r_step);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize - 1, i + j);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(levelsize - 1, i + j + 1);
			}
		}
	}

	// bottom edge
	if (deg_bottom < levelsize) {
		int b_step = levelsize / deg_bottom;

		for (int i = 0; i < levelsize; i += b_step) {
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i, levelsize);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i + b_step, levelsize);
			idata[numwritten++] = CALC_BOUNDARY_INDEX(i + b_step / 2, levelsize - 1);

			for (int j = 0; j < b_step / 2; ++j) {
				if (i == 0 && j == 0 && deg_left < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(i, levelsize);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j + 1, levelsize - 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j, levelsize - 1);
			}

			for (int j = b_step / 2; j < b_step; ++j) {
				if (i == levelsize - b_step && j == b_step - 1 && deg_right < levelsize)
					continue;

				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + b_step, levelsize);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j + 1, levelsize - 1);
				idata[numwritten++] = CALC_BOUNDARY_INDEX(i + j, levelsize - 1);
			}
		}
	}

	return numwritten;
}

void FourierTransform(GLuint spectrum)
{
	// horizontal pass
	glBindImageTexture(0, spectrum, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, tempdata, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RG32F);

	glUseProgram(fourierFFTShaderProgramObject);
	{
		glDispatchCompute(DISP_MAP_SIZE, 1, 1);
	}
	glUseProgram(0);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// vertical pass
	glBindImageTexture(0, tempdata, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, spectrum, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RG32F);

	glUseProgram(fourierFFTShaderProgramObject);
	{
		glDispatchCompute(DISP_MAP_SIZE, 1, 1);
	}
	glUseProgram(0);
}

void renderOcean(unsigned int envmap, float alpha, float elapsedtime)
{
	//code
	static float time = 0.0f;

	Math::Matrix world, view, proj;
	Math::Matrix viewproj, debugviewproj;
	Math::Vector3 eye, debugeye;

	
	/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screenwidth, screenheight);*/

	//if (currentSceneNumber == SCENE_OCEAN_WITH_HAJI_ALI)
	//{
	//	if (cameraSetForHajiAli == FALSE)
	//	{
	//		setCameraForHajiAli();
	//		cameraSetForHajiAli = TRUE;
	//	}
	//}
	//else if (currentSceneNumber == SCENE_OCEAN_WITH_SHIPS)
	//{
	//	if (cameraSetForShipsScene == FALSE)
	//	{
	//		setCameraForShipsScene();
	//		cameraSetForShipsScene = TRUE;
	//	}
	//}

	camera.Animate(alpha);
	camera.GetViewMatrix(view);
	camera.GetProjectionMatrix(proj);
	camera.GetEyePosition(eye);

	Math::MatrixMultiply(viewproj, view, proj);

	// update spectra
	glUseProgram(updateSpectrumShaderProgramObject);
	glUniform1f(updateSpectrumTimeUniform, time);

	glBindImageTexture(0, initial, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, frequencies, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);

	glBindImageTexture(2, updated[0], 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RG32F);
	glBindImageTexture(3, updated[1], 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RG32F);

	glDispatchCompute(DISP_MAP_SIZE / 16, DISP_MAP_SIZE / 16, 1);
	
	glUseProgram(0);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// transform spectra to spatial/time domain
	FourierTransform(updated[0]);
	FourierTransform(updated[1]);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// calculate displacement map
	glBindImageTexture(0, updated[0], 0, GL_TRUE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, updated[1], 0, GL_TRUE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(2, displacement, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glUseProgram(displacementShaderProgramObject);
	{
		glDispatchCompute(DISP_MAP_SIZE / 16, DISP_MAP_SIZE / 16, 1);
	}
	glUseProgram(0);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// calculate normal & folding map
	glBindImageTexture(0, displacement, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, gradients, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);

	glUseProgram(gradientsShaderProgramObject);
	{
		glDispatchCompute(DISP_MAP_SIZE / 16, DISP_MAP_SIZE / 16, 1);
	}
	glUseProgram(0);

	glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

	glBindTexture(GL_TEXTURE_2D, gradients);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// render sky
	glEnable(GL_FRAMEBUFFER_SRGB);

	Math::MatrixScaling(world, 5.0f, 5.0f, 5.0f);

	world._41 = eye.x;
	world._42 = eye.y;
	world._43 = eye.z;
		
	// build quadtree
	tree.Rebuild(viewproj, proj, eye);

	// render ocean
	Math::Matrix	flipYZ = { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 };
	Math::Matrix	localtraf;
	Math::Vector4	uvparams = { 0, 0, 0, 0 };
	Math::Vector4	perlinoffset = { 0, 0, 0, 0 };
	Math::Vector2	w = WIND_DIRECTION;
	int				pattern[4];
	GLuint			subset = 0;

	uvparams.x = 1.0f / PATCH_SIZE;
	uvparams.y = 0.5f / DISP_MAP_SIZE;

	perlinoffset.x = -w.x * time * 0.06f;
	perlinoffset.y = -w.y * time * 0.06f;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glUseProgram(oceanShaderProgramObject);

	glUniformMatrix4fv(matViewProjUniform, 1, GL_FALSE, viewproj);
	glUniform2fv(perlinOffsetUniform, 1, perlinoffset);
	glUniform3fv(eyePosUniform, 1, eye);
	glUniform3fv(oceanColorUniform, 1, OceanColors[currentcolor]);
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, displacement);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, perlintex);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envmap);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gradients);

		tree.Traverse([&](const OceanQuadMesh::Node& node) {
			float levelsize = (float)(MESH_SIZE >> node.lod);
			float scale = node.length / levelsize;

			Math::MatrixScaling(localtraf, scale, scale, 0);

			Math::MatrixTranslation(world, node.start[0], 0, node.start[1]);
			Math::MatrixMultiply(world, flipYZ, world);

			uvparams.z = node.start[0] / PATCH_SIZE;
			uvparams.w = node.start[1] / PATCH_SIZE;

			glUniformMatrix4fv(matLocalUniform, 1, GL_FALSE, localtraf);
			glUniformMatrix4fv(matWorldUniform, 1, GL_FALSE, world);
			glUniform4fv(uvParamsUniform, 1, uvparams);

			tree.FindSubsetPattern(pattern, node);
			subset = CalcSubsetIndex(node.lod, pattern[0], pattern[1], pattern[2], pattern[3]);

			if (subset < oceanmesh->GetNumSubsets() - 1) {
				oceanmesh->DrawSubset(subset);
				oceanmesh->DrawSubset(subset + 1);
			}
		});

		glActiveTexture(GL_TEXTURE0);
	}
	glUniform1f(sunPositionUniform, fSunPosition);

	glUseProgram(0);

	/*if (use_debug)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/

	glDisable(GL_FRAMEBUFFER_SRGB);
	time += elapsedtime;

	// TODO: draw spectra
		
	// reset states
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	if (fSunPosition < -0.761f)
	{
		fSunPosition += 0.0001f;
	}
}

void callbackOceanShader(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//code
	switch (iMsg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_NUMPAD0:
			currentcolor++;
			if (currentcolor > 7)
			{
				currentcolor = 0;
			}
			break;

		case 'W':
			camera.MoveCameraForward();
			break;

		case 'S':
			camera.MoveCameraBackward();
			break;

		case 'A':
			camera.MoveCameraLeft();
			break;

		case 'D':
			camera.MoveCameraRight();
			break;

		case 'U':
			yawValue += 0.1f;
			break;

		case 'I':
			yawValue -= 0.1f;
			break;

		case 'O':
			pitchValue += 0.1f;
			break;

		case 'P':
			pitchValue -= 0.1f;
			break;

		case '1':
			eyePos_X += 0.1f;
			break;

		case '2':
			eyePos_X -= 0.1f;
			break;

		case '3':
			eyePos_Y += 0.1f;
			break;

		case '4':
			eyePos_Y -= 0.1f;
			break;

		case '5':
			eyePos_Z += 0.1f;
			break;

		case '6':
			eyePos_Z -= 0.1f;
			break;

		case '7':
			O_FOVY += 0.1f;
			break;

		case '8':
			O_FOVY -= 0.1f;
			break;

		case VK_HOME:
			PrintLog("yawValue: %f\n", yawValue);
			PrintLog("pitchValue: %f\n", pitchValue);
			PrintLog("eyePos_X = %ff; eyePos_Y = %ff; eyePos_Z = %ff;\n", eyePos_X, eyePos_Y, eyePos_Z);
			PrintLog("O_FOVY: %ff\n", O_FOVY);
		}
		break;

	case WM_KEYUP:
		switch (wParam)
		{
		case 'W':
			camera.MoveCameraForwardR();
			break;

		case 'S':
			camera.MoveCameraBackwardR();
			break;

		case 'A':
			camera.MoveCameraLeftR();
			break;

		case 'D':
			camera.MoveCameraRightR();
			break;
		}
		break;

	default:
		break;
	}
}

void moveOceanCameraForward(void)
{
	//code
	static float cameraSpeed = 0.01f;
	static float cameraDistance = 0.0f;

	if (cameraDistance <= 2.0f)
	{
		camera.MoveCameraForward();
		cameraDistance += cameraSpeed;
	}
	else
	{
		camera.MoveCameraForwardR();
	}
}

void moveOceanCameraBackward(void)
{
	//code
	static float cameraSpeed = 0.01f;
	static float cameraDistance = 2.0f;

	if (cameraDistance >= 0.0f)
	{
		camera.MoveCameraBackward();
		cameraDistance -= cameraSpeed;
	}
	else
	{
		camera.MoveCameraBackwardR();
	}
}

void updateOceanShader(void)
{
	//code
	camera.Update(0.0333f);
}