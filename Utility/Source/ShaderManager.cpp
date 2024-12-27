#include "../../Source/Globals.h" 
#include"../Headers//ShaderManager.h" 


//constructor
ShaderManager::ShaderManager() {

	//initializing
	status = 0;
	infoLogLength = 0;
	log = NULL;
	FILE* fp = NULL;
	size_t fileSizeLength = 0;
}
ShaderManager::~ShaderManager() {

	if (log) {
		delete(log);
		log = NULL;
	}
	if (fp) {
		delete(fp);
		fp = NULL;
	}
}
//method defination
GLuint ShaderManager::loadShaderAndCompileShader(const std::vector<std::string>shaderName) {

	//code
	
	//Creating program
	shaderProgramObject = glCreateProgram();

	//Compiling shader one by one.
	for (int i = 0; i < shaderName.size(); i++) {

		if (fopen_s(&fp, shaderName.at(i).c_str(), "rb") != 0) {
			fprintf(gpFile, "Loading the %s shader file failed\n", shaderName.at(i).c_str());
		}

		fseek(fp, 0, SEEK_END);
		fileSizeLength = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		char* fileContent = (char*)malloc(fileSizeLength + 1);
		if (fileContent == NULL) {
			fprintf(gpFile, "Failed to create memory\n");
			fclose(fp);
			fp = NULL;
			fileContent = NULL;
			return -1;
		}
		if (fread(fileContent, fileSizeLength, 1, fp) != 1) {
			fprintf(gpFile, "Failed to load the file\n");
			fclose(fp);
			fp = NULL;
			fileContent = NULL;
			return -1;
		}

		fileContent[fileSizeLength] = '\0';

		fclose(fp);
		fp = NULL;

		//getting shader object for each shader
		GLuint shaderObject = getshaderObject(shaderName.at(i));
 		if (shaderObject == 0) {
			fprintf(gpFile, "Failed to create shader object\n");
			return -1;
		}

		glShaderSource(shaderObject, 1, (const GLchar**)&fileContent, NULL);
		
		glCompileShader(shaderObject);
		glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
		{
			glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				log = (char*)malloc(sizeof(char) * infoLogLength);
				if (log != NULL)
				{
					GLsizei written;
					glGetShaderInfoLog(shaderObject, infoLogLength, &written, log);
					fprintf(gpFile, "Failed to compiler %s shader with log error %s\n", shaderName.at(i), log);
					free(log);
					return -1;

				}
			}
		}
		//Attahced shader one by one to program
		glAttachShader(shaderProgramObject,shaderObject);

		free(fileContent);
	}

	//linking the program
	glLinkProgram(shaderProgramObject);
	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0) {
			log = (char*)malloc(sizeof(char) * infoLogLength);
			GLsizei written;
			glGetProgramInfoLog(shaderProgramObject, infoLogLength, &written, log);
			fprintf(gpFile, "Shader Program Linking Log : %s\n", log);
			free(log);
			return -1;
		}
	}
	return 0;
}

void ShaderManager::useProgram() {

	glUseProgram(shaderProgramObject);
}
void ShaderManager::unUseProgram() {

	glUseProgram(0);
}

void ShaderManager::setMat4(const std::string uniformString, const mat4& value) {

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramObject, uniformString.c_str()), 1, GL_FALSE, &value[0][0]);
}
void ShaderManager::setInt(const std::string &uniformString, const int value) {
	glUniform1i(glGetUniformLocation(shaderProgramObject,uniformString.c_str()),value);
}

void ShaderManager::setFloat(const std::string& uniformString, const float value) {
	glUniform1f(glGetUniformLocation(shaderProgramObject, uniformString.c_str()), value);
}

void ShaderManager::setVec2(const std::string& uniformString, const vec2& value) {
	glUniform2fv(glGetUniformLocation(shaderProgramObject, uniformString.c_str()), 1, &value[0]);
}
void ShaderManager::setVec2(const std::string& uniformString, float x, float y) {
	glUniform2f(glGetUniformLocation(shaderProgramObject, uniformString.c_str()), x, y);
}

void ShaderManager::setVec3(const std::string& uniformString, const vec3& value) {
	glUniform3fv(glGetUniformLocation(shaderProgramObject, uniformString.c_str()), 1, &value[0]);
}
void ShaderManager::setVec3(const std::string& uniformString, float x, float y,float w) {
	glUniform3f(glGetUniformLocation(shaderProgramObject, uniformString.c_str()), x, y,w);
}

void ShaderManager::setVec4(const std::string& uniformString, const vec4 &value) {
	glUniform4fv(glGetUniformLocation(shaderProgramObject, uniformString.c_str()), 1,&value[0]);
}
void ShaderManager::setVec4(const std::string& uniformString, float x,float y,float z,float w) {
	glUniform4f(glGetUniformLocation(shaderProgramObject, uniformString.c_str()), x,y,z,w);
}


GLuint ShaderManager::getshaderObject(std::string shaderName) {

	int pos = shaderName.find('.');
	std::string shaderType = shaderName.substr(pos + 1);
	
	if (shaderType.compare("vert")==0 )
		return glCreateShader(GL_VERTEX_SHADER);
	else if (shaderType.compare("frag")==0)
		return glCreateShader(GL_FRAGMENT_SHADER);
	else if (shaderType.compare("tese") == 0)
		return glCreateShader(GL_TESS_EVALUATION_SHADER);
	else if (shaderType.compare("tesc") == 0)
		return glCreateShader(GL_TESS_CONTROL_SHADER);

	return -1;
}

GLuint ShaderManager::getShaderProgramObject() {
	return shaderProgramObject;
}
void ShaderManager::uninitializeShader() {

	//variable declaration 
	GLsizei numAttachShader;

	//code
	if (shaderProgramObject) {
		glUseProgram(shaderProgramObject);
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numAttachShader);
		GLuint* shaderObject = NULL;
		shaderObject = (GLuint*)malloc(numAttachShader * sizeof(GLuint));

		glGetAttachedShaders(shaderProgramObject, numAttachShader, &numAttachShader, shaderObject);

		for (GLsizei i = 0; i < numAttachShader; i++) {

			glDetachShader(shaderProgramObject, shaderObject[i]);
			glDeleteShader(shaderObject[i]);
			shaderObject[i] = 0;

		}
		free(shaderObject);
		shaderObject = NULL;
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
	}
}
