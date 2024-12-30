#include "Terrain.h"


int TerrainShader::initializeTerrainShaderProgram(const char* filename) {
	std::vector<std::string> shaderName;
	TextureManager* hightmapTexture = new TextureManager();


	shaderName.push_back("Shaders\\Terrain\\terrain.vert");
	shaderName.push_back("Shaders\\Terrain\\terrain.tesc");
	shaderName.push_back("Shaders\\Terrain\\terrain.tese");
	shaderName.push_back("Shaders\\Terrain\\terrain.frag");

	int retVal = shaderProgramObject.loadShaderAndCompileShader(shaderName);
	if (retVal != 0) {
		PrintLog("loading of Terrain shader failed\n");
		return -1;
	}

	hightmapTexture->loadTerrainTexture(filename, &mesh_width, &mesh_height, &height_texture);

	//setup vertex data 
	for (int i = 0; i < (int)(mesh_width / PATCH_SIZE); i++)
	{
		for (int j = 0; j < (int)(mesh_height / PATCH_SIZE); j++)
		{
			GLfloat x = j * PATCH_SIZE / (GLfloat)mesh_height;
			GLfloat z = i * PATCH_SIZE / (GLfloat)mesh_width;
			vertices.push_back(x);
			vertices.push_back(z);
		}
	}

	glGenVertexArrays(1, &vao_terrain);
	glBindVertexArray(vao_terrain);
	glGenBuffers(1, &vbo_terrain_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_terrain_position);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(SHADER_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SHADER_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	return 0;

}

void TerrainShader::useTerrainShaderProgram() {
	glFrontFace(GL_CCW);
	shaderProgramObject.useProgram();
}
void TerrainShader::displayTerrainShader(TextureManager* terrainTextures,mat4 modelMatrix,mat4 viewMatrix,vec3 camPos,GLfloat hightStep =165,GLfloat gridSpace =1.35) {

	shaderProgramObject.setMat4("model_matrix", modelMatrix);
	shaderProgramObject.setMat4("view_matrix", viewMatrix);
	shaderProgramObject.setMat4("projection_matrix", prespectiveProjectionMatrix);

	shaderProgramObject.setFloat("height_step", hightStep);
	shaderProgramObject.setFloat("grid_spacing", gridSpace);
	shaderProgramObject.setFloat("tex_tiling_factor", 6.0f);
	shaderProgramObject.setInt("scale_factor", 1);

	//shaderProgramObject.setVec3("light_position", 114.318443, 10000+12.23, 189.78);
	shaderProgramObject.setVec3("light_position", lightPos);

	shaderProgramObject.setVec3("viewPosUniform", camPos);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, height_texture);
	shaderProgramObject.setInt("height_map", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, terrainTextures->getTexture("splat"));
	shaderProgramObject.setInt("splat_map", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, terrainTextures->getTexture("grass5"));
	shaderProgramObject.setInt("grass_sampler", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, terrainTextures->getTexture("NormalMap"));
	shaderProgramObject.setInt("grass_normal", 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, terrainTextures->getTexture("rock"));
	shaderProgramObject.setInt("rock_sampler", 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, terrainTextures->getTexture("rock_normal"));
	shaderProgramObject.setInt("rock_normal", 5);

	glBindVertexArray(vao_terrain);
	glPatchParameteri(GL_PATCH_VERTICES, 1);
	glDrawArrays(GL_PATCHES, 0, (GLsizei)vertices.size() / 2);
	glBindVertexArray(0);

}
void TerrainShader::unUseTerrainShaderProgram() {
	shaderProgramObject.unUseProgram();

}
void TerrainShader::deleteShaderProgramObject() {
	shaderProgramObject.uninitializeShader();
}