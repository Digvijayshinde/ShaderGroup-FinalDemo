
#include "../../Source/Globals.h"
#include "../Headers/Renderers.h"

GLuint VBO_CubePosition;
GLuint VBO_CubeColor;
GLuint VBO_CubeTexcoord;
GLuint VBO_CubeNormal;
GLuint VAO_Cube;


int Renderers::initializeCubeRenderer() {

	//code
	const GLfloat cubePosition[] =
	{
			// front
		 1.0f,  1.0f,  1.0f, // top-right of front
		-1.0f,  1.0f,  1.0f, // top-left of front
		-1.0f, -1.0f,  1.0f, // bottom-left of front
		 1.0f, -1.0f,  1.0f, // bottom-right of front

	 // right
	  1.0f,  1.0f, -1.0f, // top-right of right
	  1.0f,  1.0f,  1.0f, // top-left of right
	  1.0f, -1.0f,  1.0f, // bottom-left of right
	  1.0f, -1.0f, -1.0f, // bottom-right of right

	  // back
	   1.0f,  1.0f, -1.0f, // top-right of back
	  -1.0f,  1.0f, -1.0f, // top-left of back
	  -1.0f, -1.0f, -1.0f, // bottom-left of back
	   1.0f, -1.0f, -1.0f, // bottom-right of back

	   // left
	   -1.0f,  1.0f,  1.0f, // top-right of left
	   -1.0f,  1.0f, -1.0f, // top-left of left
	   -1.0f, -1.0f, -1.0f, // bottom-left of left
	   -1.0f, -1.0f,  1.0f, // bottom-right of left

	   // top
		1.0f,  1.0f, -1.0f, // top-right of top
	   -1.0f,  1.0f, -1.0f, // top-left of top
	   -1.0f,  1.0f,  1.0f, // bottom-left of top
		1.0f,  1.0f,  1.0f, // bottom-right of top

		// bottom
		 1.0f, -1.0f,  1.0f, // top-right of bottom
		-1.0f, -1.0f,  1.0f, // top-left of bottom
		-1.0f, -1.0f, -1.0f, // bottom-left of bottom
		 1.0f, -1.0f, -1.0f, // bottom-right of bottom
	};

	const GLfloat cubeColor[] =
	{
		// front
		1.0f, 0.0f, 0.0f, // top-right of front
		1.0f, 0.0f, 0.0f, // top-left of front
		1.0f, 0.0f, 0.0f, // bottom-left of front
		1.0f, 0.0f, 0.0f, // bottom-right of front

		// right
		0.0f, 0.0f, 1.0f, // top-right of right
		0.0f, 0.0f, 1.0f, // top-left of right
		0.0f, 0.0f, 1.0f, // bottom-left of right
		0.0f, 0.0f, 1.0f, // bottom-right of right

		// back
		1.0f, 1.0f, 0.0f, // top-right of back
		1.0f, 1.0f, 0.0f, // top-left of back
		1.0f, 1.0f, 0.0f, // bottom-left of back
		1.0f, 1.0f, 0.0f, // bottom-right of back

		// left
		1.0f, 0.0f, 1.0f, // top-right of left
		1.0f, 0.0f, 1.0f, // top-left of left
		1.0f, 0.0f, 1.0f, // bottom-left of left
		1.0f, 0.0f, 1.0f, // bottom-right of left

		// top
		0.0f, 1.0f, 0.0f, // top-right of top
		0.0f, 1.0f, 0.0f, // top-left of top
		0.0f, 1.0f, 0.0f, // bottom-left of top
		0.0f, 1.0f, 0.0f, // bottom-right of top

		// bottom
		1.0f, 0.5f, 0.0f, // top-right of bottom
		1.0f, 0.5f, 0.0f, // top-left of bottom
		1.0f, 0.5f, 0.0f, // bottom-left of bottom
		1.0f, 0.5f, 0.0f, // bottom-right of bottom
	};

	const GLfloat cubeTexCoords[] =
	{
			// front
		1.0f, 1.0f, // top-right of front
		0.0f, 1.0f, // top-left of front
		0.0f, 0.0f, // bottom-left of front
		1.0f, 0.0f, // bottom-right of front

		// right
		1.0f, 1.0f, // top-right of right
		0.0f, 1.0f, // top-left of right
		0.0f, 0.0f, // bottom-left of right
		1.0f, 0.0f, // bottom-right of right

		// back
		1.0f, 1.0f, // top-right of back
		0.0f, 1.0f, // top-left of back
		0.0f, 0.0f, // bottom-left of back
		1.0f, 0.0f, // bottom-right of back

		// left
		1.0f, 1.0f, // top-right of left
		0.0f, 1.0f, // top-left of left
		0.0f, 0.0f, // bottom-left of left
		1.0f, 0.0f, // bottom-right of left

		// top
		1.0f, 1.0f, // top-right of top
		0.0f, 1.0f, // top-left of top
		0.0f, 0.0f, // bottom-left of top
		1.0f, 0.0f, // bottom-right of top

		// bottom
		1.0f, 1.0f, // top-right of bottom
		0.0f, 1.0f, // top-left of bottom
		0.0f, 0.0f, // bottom-left of bottom
		1.0f, 0.0f, // bottom-right of bottom
	};

	const GLfloat cubeNormal[] =
	{
			// front surface
		0.0f,  0.0f,  1.0f, // top-right of front
		0.0f,  0.0f,  1.0f, // top-left of front
		0.0f,  0.0f,  1.0f, // bottom-left of front
		0.0f,  0.0f,  1.0f, // bottom-right of front

		// right surface
		1.0f,  0.0f,  0.0f, // top-right of right
		1.0f,  0.0f,  0.0f, // top-left of right
		1.0f,  0.0f,  0.0f, // bottom-left of right
		1.0f,  0.0f,  0.0f, // bottom-right of right

		// back surface
		0.0f,  0.0f, -1.0f, // top-right of back
		0.0f,  0.0f, -1.0f, // top-left of back
		0.0f,  0.0f, -1.0f, // bottom-left of back
		0.0f,  0.0f, -1.0f, // bottom-right of back

		// left surface
	   -1.0f,  0.0f,  0.0f, // top-right of left
	   -1.0f,  0.0f,  0.0f, // top-left of left
	   -1.0f,  0.0f,  0.0f, // bottom-left of left
	   -1.0f,  0.0f,  0.0f, // bottom-right of left

	   // top surface
	   0.0f,  1.0f,  0.0f, // top-right of top
	   0.0f,  1.0f,  0.0f, // top-left of top
	   0.0f,  1.0f,  0.0f, // bottom-left of top
	   0.0f,  1.0f,  0.0f, // bottom-right of top

	   // bottom surface
	   0.0f, -1.0f,  0.0f, // top-right of bottom
	   0.0f, -1.0f,  0.0f, // top-left of bottom
	   0.0f, -1.0f,  0.0f, // bottom-left of bottom
	   0.0f, -1.0f,  0.0f, // bottom-right of bottom
	};

	glGenVertexArrays(1, &VAO_Cube);
	glBindVertexArray(VAO_Cube);
	//position
		glGenBuffers(1, &VBO_CubePosition);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_CubePosition);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cubePosition), cubePosition, GL_STATIC_DRAW);
			glVertexAttribPointer(SHADER_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(SHADER_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//color
		glGenBuffers(1, &VBO_CubeColor);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_CubeColor);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColor), cubeColor, GL_STATIC_DRAW);
			glVertexAttribPointer(SHADER_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(SHADER_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		//Normals
		glGenBuffers(1, &VBO_CubeNormal);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_CubeNormal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormal), cubeNormal, GL_STATIC_DRAW);
		glVertexAttribPointer(SHADER_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(SHADER_ATTRIBUTE_NORMAL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Texcoord
		glGenBuffers(1, &VBO_CubeTexcoord);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_CubeTexcoord);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexCoords), cubeTexCoords, GL_STATIC_DRAW);
			glVertexAttribPointer(SHADER_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(SHADER_ATTRIBUTE_TEXTURE0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		

	glBindVertexArray(0);
	return 0;
}

void Renderers::renderCube() {

	//code

	glBindVertexArray(VAO_Cube);
		//here the drawing start
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);

}

void Renderers::unitializeCube() {

	//code
	if (VAO_Cube) {
		glDeleteBuffers(1, &VAO_Cube);
		VAO_Cube = 0;
	}
	if (VBO_CubeNormal) {
		glDeleteBuffers(1, &VBO_CubeNormal);
		VBO_CubeNormal = 0;
	}
	if (VBO_CubeTexcoord) {
		glDeleteBuffers(1, &VBO_CubeTexcoord);
		 VBO_CubeTexcoord = 0;
	}
	if (VBO_CubeColor) {
		glDeleteBuffers(1, &VBO_CubeColor);
		VBO_CubeColor = 0;
	}
	if (VBO_CubePosition) {
		glDeleteBuffers(1, &VBO_CubePosition);
		VBO_CubePosition = 0;
	}
}

