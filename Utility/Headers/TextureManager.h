#pragma once
#include "../../Source/Globals.h"
#include <map>		//required for getting map reference.

/*
	Texture manager class help to track the texture per scene   ......This will change in furture  if required.
	map is use to store the  file name and id. only single copyof texture is created per scene as we using map.
*/

class TextureManager {

private:
	std::map<std::string, GLuint> textureCollection;
	std::map<unsigned int, GLuint> textureWidth;
	std::map<unsigned int, GLuint> textureHight;

	GLuint loadSTBTextureFromFile(const char*);
	GLuint loadBmpTextureFromFile(const char*, TCHAR imageResourceId[]);
	GLuint loadDDSTextureFromFile(const char* );

public:
	TextureManager();
	~TextureManager();

	void storeTextureFromFile(std::string,std::string,int resource=-1);
	GLuint getTexture(std::string);

	void removeTextueCollection();
	unsigned int loadCubeMapTexture(std::vector<std::string>);
	void loadTerrainTexture(const char* filename, GLuint* tex_width, GLuint* tex_height, GLuint* texture);
	GLuint mesh_width;
	GLuint mesh_height;
};