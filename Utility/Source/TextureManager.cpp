#include "../Headers/TextureManager.h"
#include "../Headers/DDSLoader.h"
TextureManager::TextureManager() = default;
TextureManager::~TextureManager(){}

/******************************************************
Only of rthe .bmp texture file resource is is use.
For the ddds and othe formate image this parameter is set a default value.
******************************************************/

void TextureManager::storeTextureFromFile(std::string dir, std::string fileName,int resourceId) {

	//code
	std::string fullFilePath = dir +"\\"+ fileName;
	int pos = fileName.find('.');
	std::string fileNameWithoutExtension = fileName.substr(0, pos);

	if (textureCollection.find(fileNameWithoutExtension) == textureCollection.end())
	{
		std::string imageType = fileName.substr(pos + 1);

		if (imageType.compare("dds") == 0)
			textureCollection[fileNameWithoutExtension]=loadDDSTextureFromFile(fullFilePath.c_str(),isDDSTextureClipped);

		else if (imageType.compare("bmp") == 0 && resourceId!=-1)
			textureCollection[fileNameWithoutExtension]=loadBmpTextureFromFile(fullFilePath.c_str(),MAKEINTRESOURCE(resourceId));

		else
			textureCollection[fileNameWithoutExtension] = loadSTBTextureFromFile(fullFilePath.c_str());
	}
}

GLuint TextureManager::loadSTBTextureFromFile(const char* fileName) {

	//variable declaration
	unsigned int textureID = 0;
	int width, height, nrComponents;

	//code
	glGenTextures(1, &textureID);
	unsigned char* data = stbi_load(fileName, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glGenerateTextureMipmap(textureID);
		glBindTexture(GL_TEXTURE_2D, NULL);

	}
	else
	{
		fprintf(gpFile, "Texture failed to load \n");
	}

	return textureID;
}


GLuint TextureManager::loadBmpTextureFromFile(const char* fileName, TCHAR imageResourceId[]) {

	//variable declaration
	HBITMAP hBitMap = NULL;
	BITMAP bmp;
	BOOL bResult = FALSE;
	GLuint texture = NULL;

	//code
	hBitMap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (hBitMap) {

		bResult = TRUE;
		GetObject(hBitMap, sizeof(bmp), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		DeleteObject(hBitMap);
	}
	return texture;
}

GLuint TextureManager::loadDDSTextureFromFile(const char* fileName, bool isDDSTextureClipped) {

	imageData* image = NULL;

	return  loadDDSTexture_Custom(fileName, image, isDDSTextureClipped);
}

GLuint TextureManager:: getTexture(std::string keyName) {

	auto it = textureCollection.find(keyName);
	if (it == textureCollection.end()) {
		fprintf(gpFile,"Texture with name %s is not found\n", keyName);
		return 0;
	}
	else {
		return it->second;
	}
}
unsigned int TextureManager::loadCubeMapTexture(std::vector<std::string> facesCubeMap)
{
	unsigned int textureIDCubeMap;
	glGenTextures(1, &textureIDCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureIDCubeMap);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < facesCubeMap.size(); i++)
	{
		unsigned char* data = stbi_load(facesCubeMap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			//std::cout << "Cubemap tex failed to load at path: " << facesCubeMap[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureIDCubeMap;
}

void TextureManager::removeTextueCollection() {

	//Remove all tetxure sample id
	for (auto i = textureCollection.begin(); i != textureCollection.end(); i++) {
		glDeleteTextures(1,&i->second);
		i->second = NULL;
	}
	//clear the map
	textureCollection.clear();
}

void TextureManager::loadTerrainTexture(const char* filename, GLuint* tex_width, GLuint* tex_height, GLuint* texture)
{
	//variable declarations
	int width, height;
	unsigned char* pixel_data = NULL;

	//code
	pixel_data = stbi_load(filename, &width, &height, NULL, 0);
	if (pixel_data == NULL)
	{
		PrintLog("failed to load texture.\n");
		return;
	}
	PrintLog("load texture successfull %s.\n", filename);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	//set up texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//push the data to texture memory
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLint)width, (GLint)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)pixel_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	if (tex_width)
	{
		*tex_width = width;
	}

	if (tex_height)
	{
		*tex_height = height;
	}

	stbi_image_free(pixel_data);
	pixel_data = NULL;
}

