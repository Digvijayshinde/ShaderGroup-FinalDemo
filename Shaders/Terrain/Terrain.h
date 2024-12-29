#pragma once
#pragma once
#include "../../Utility/Headers/ShaderManager.h"
#include "../../Source/Common.h"

#define PATCH_SIZE  64

class TerrainShader {
private:
	ShaderManager shaderProgramObject;
	std::vector<GLfloat> vertices;
	GLuint mesh_width;
	GLuint mesh_height;
	GLuint vao_terrain;
	GLuint vbo_terrain_position;
	GLfloat gridSpace = 0.5f;
	GLfloat hightStep = 70.89f;
	GLfloat scaleTerrain = 0.5f;
	GLuint height_texture;
public:
	int initializeTerrainShaderProgram(const char* filename);
	void useTerrainShaderProgram();
	void displayTerrainShader(TextureManager* terrainTextures, mat4 modelMatrix,mat4,vec3,GLfloat,GLfloat);
	void unUseTerrainShaderProgram();
	void deleteShaderProgramObject();
	vec3 lightPos = vec3(500.0, 10150.0, 500.0);

};