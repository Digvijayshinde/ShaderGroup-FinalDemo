#include"../../Source/Common.h"
#include "../../Source/Globals.h"
#include "../../Renderres/Headers/Renderers.h"

GLuint VBO_QuadPosition;
GLuint VBO_QuadColor;
GLuint VBO_QuadTexCoord;
GLuint VBO_QuadNormal;
GLuint VAO_Quad;


int Renderers::initializeQuadRenderer() {

	//code
	const GLfloat quadPosition[] =
	{
		//front
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};
	const GLfloat quadColor[] =
	{
		//front
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f
	};
	const GLfloat QuadNormals[]{
		//front
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	};

	const GLfloat quadTexCoords[] =
	{
		//front
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};
//position
	glGenVertexArrays(1, &VAO_Quad);
	glBindVertexArray(VAO_Quad);
		glGenBuffers(1, &VBO_QuadPosition);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_QuadPosition);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadPosition), quadPosition, GL_STATIC_DRAW);
			glVertexAttribPointer(SHADER_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(SHADER_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//color
		glGenBuffers(1, &VBO_QuadColor);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_QuadColor);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadColor), quadColor, GL_STATIC_DRAW);
			glVertexAttribPointer(SHADER_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(SHADER_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Texcoord
		glGenBuffers(1, &VBO_QuadTexCoord);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_QuadTexCoord);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadTexCoords), quadTexCoords, GL_STATIC_DRAW);
			glVertexAttribPointer(SHADER_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(SHADER_ATTRIBUTE_TEXTURE0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Normals
		glGenBuffers(1, &VBO_QuadNormal);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_QuadNormal);
			glBufferData(GL_ARRAY_BUFFER, sizeof(QuadNormals), QuadNormals, GL_STATIC_DRAW);
			glVertexAttribPointer(SHADER_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(SHADER_ATTRIBUTE_NORMAL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	return 0;
}

void Renderers::renderQuad() {

	//code
	glBindVertexArray(VAO_Quad);
	//here the drawing start
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);

}

void Renderers::unitializeQuad() {

	//code
	if (VAO_Quad) {
		glDeleteBuffers(1, &VAO_Quad);
		VAO_Quad = 0;
	}
	if (VBO_QuadNormal) {
		glDeleteBuffers(1, &VBO_QuadNormal);
		VBO_QuadNormal = 0;
	}
	if (VBO_QuadTexCoord) {
		glDeleteBuffers(1, &VBO_QuadTexCoord);
		VBO_QuadTexCoord = 0;
	}
	if (VBO_QuadColor) {
		glDeleteBuffers(1, &VBO_QuadColor);
		VBO_QuadColor = 0;
	}
	if (VBO_QuadPosition) {
		glDeleteBuffers(1, &VBO_QuadPosition);
		VBO_QuadPosition = 0;
	}
}

