#pragma once
#include"../../Source/Globals.h"

class ShaderManager {

	//variable declaration
	int status;
	int infoLogLength;
	char* log;
	FILE* fp;
	size_t fileSizeLength;
	GLuint shaderProgramObject;

public:
		ShaderManager();
		~ShaderManager();

		GLuint loadShaderAndCompileShader(const std::vector<std::string>);
		void useProgram();
		void unUseProgram();

		void setMat4(const std::string uniformString, const mat4& value, int instanceCount=1);
		void setInt(const std::string &uniformString, const int value);
		void setFloat(const std::string& uniformString, const float value);



		void setVec2(const std::string& uniformString, const vec2& value);
		void setVec2(const std::string& uniformString, float x, float y);


		void setVec3(const std::string& uniformString, const vec3 &value);
		void setVec3(const std::string& uniformString, float x, float y,float z);

		void setVec4(const std::string& uniformString, const vec4& value);
		void setVec4(const std::string& uniformString, float x, float y, float z,float w);
		GLuint getShaderProgramObject();
		void uninitializeShader();

private:
		GLuint getshaderObject(std::string shaderName);

};

