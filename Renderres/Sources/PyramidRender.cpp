#include"../../Source/Common.h"
#include "../../Source/Globals.h"
#include "../../Renderres/Headers/Renderers.h"

GLuint VBO_PyramidPosition;
GLuint VBO_PyramidColor;
GLuint VBO_PyramidTexCoord;
GLuint VBO_PyramidNormal;
GLuint VAO_Pyramid;


int Renderers::initializePyramidRenderer() {

	//code

	const GLfloat pyramidPosition[] =
	{
		//front triangle
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		//right triangle
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,

		//back triangle
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,

		//left triangle
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f
	};
	// color
	const GLfloat pyramidColors[] =
	{
		// front
		1.0f, 0.0f, 0.0f, // front-top
		1.0f, 0.0f, 0.0f, // front-left
		1.0f, 0.0f, 0.0f,  // front-right

		// right
		0.0f, 1.0f, 0.0f, // right-top
		0.0f, 1.0f, 0.0f, // right-left
		0.0f, 1.0f, 0.0f, // right-right

		// back
		0.0f, 0.0f, 1.0f, // back-top
		0.0f, 0.0f, 1.0f, // back-left
		0.0f, 0.0f, 1.0f, // back-right

		// left
		1.0f, 1.0f, 0.0f, // left-top
		1.0f, 1.0f, 0.0f, // left-left
		1.0f, 1.0f, 0.0f, // left-right
	};

	const GLfloat pyramidTexCoords[] =
	{
		//front triangle
		0.5f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		//right triangle
		0.5f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		//back triangle
		0.5f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		//left triangle
		0.5f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};
	
	glGenVertexArrays(1, &VAO_Pyramid);
		glBindVertexArray(VAO_Pyramid);
		//POSITION	
			glGenBuffers(1, &VBO_PyramidPosition);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_PyramidPosition);
			glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPosition), pyramidPosition, GL_STATIC_DRAW);
			glVertexAttribPointer(SHADER_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(SHADER_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//COLOR
		glGenBuffers(1, &VBO_PyramidColor);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_PyramidColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidColors), pyramidColors, GL_STATIC_DRAW);
		glVertexAttribPointer(SHADER_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(SHADER_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//TEXCOORD
		glGenBuffers(1, &VBO_PyramidTexCoord);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_PyramidTexCoord);
			glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTexCoords), pyramidTexCoords, GL_STATIC_DRAW);
			glVertexAttribPointer(SHADER_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(SHADER_ATTRIBUTE_TEXTURE0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return 0;

}

void Renderers::renderPyramid() {

	//code
	glBindVertexArray(VAO_Pyramid);
	//here the drawing start
	glDrawArrays(GL_TRIANGLES, 0, 12);
	glBindVertexArray(0);

}

void Renderers::unitializePyramid() {

	//code
	if (VAO_Pyramid) {
		glDeleteBuffers(1, &VAO_Pyramid);
		VAO_Pyramid = 0;
	}
	if (VBO_PyramidNormal) {
		glDeleteBuffers(1, &VBO_PyramidNormal);
		VBO_PyramidNormal = 0;
	}
	if (VBO_PyramidTexCoord) {
		glDeleteBuffers(1, &VBO_PyramidTexCoord);
		VBO_PyramidTexCoord = 0;
	}
	if (VBO_PyramidColor) {
		glDeleteBuffers(1, &VBO_PyramidColor);
		VBO_PyramidColor = 0;
	}
	if (VBO_PyramidPosition) {
		glDeleteBuffers(1, &VBO_PyramidPosition);
		VBO_PyramidPosition = 0;
	}
}

