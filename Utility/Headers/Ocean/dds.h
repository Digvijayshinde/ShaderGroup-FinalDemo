
#ifndef _DDS_H_
#define _DDS_H_

#include <cstdint>

enum DDS_Image_Type
{
	DDSImageType2D = 0,
	DDSImageTypeCube,
	DDSImageTypeVolume
};

struct DDS_Image_Info
{
	uint32_t		Width;
	uint32_t		Height;
	uint32_t		Depth;
	uint32_t		Format;
	uint32_t		MipLevels;
	uint32_t		DataSize;
	void*			Data;
	DDS_Image_Type	Type;
};

enum OpenGLFormat
{
	GLFMT_UNKNOWN = 0,
	GLFMT_R8,
	GLFMT_R8G8,
	GLFMT_R8G8B8,
	GLFMT_R8G8B8_sRGB,
	GLFMT_B8G8R8,
	GLFMT_B8G8R8_sRGB,
	GLFMT_A8R8G8B8,
	GLFMT_A8R8G8B8_sRGB,
	GLFMT_A8B8G8R8,
	GLFMT_A8B8G8R8_sRGB,

	GLFMT_D24S8,
	GLFMT_D32F,

	GLFMT_DXT1,
	GLFMT_DXT1_sRGB,
	GLFMT_DXT5,
	GLFMT_DXT5_sRGB,

	GLFMT_R16F,
	GLFMT_G16R16F,
	GLFMT_A16B16G16R16F,

	GLFMT_R32F,
	GLFMT_G32R32F,
	GLFMT_A32B32G32R32F
};

bool LoadFromDDS(const char* file, DDS_Image_Info* outinfo);
bool SaveToDDS(const char* file, const DDS_Image_Info* info);

uint32_t GetImageSize(uint32_t width, uint32_t height, uint32_t bytes, uint32_t miplevels);
uint32_t GetCompressedImageSize(uint32_t width, uint32_t height, uint32_t miplevels, uint32_t format);
uint32_t GetCompressedImageSize(uint32_t width, uint32_t height, uint32_t depth, uint32_t miplevels, uint32_t format);
uint32_t GetCompressedLevelSize(uint32_t width, uint32_t height, uint32_t level, uint32_t format);
uint32_t GetCompressedLevelSize(uint32_t width, uint32_t height, uint32_t depth, uint32_t level, uint32_t format);

#endif
