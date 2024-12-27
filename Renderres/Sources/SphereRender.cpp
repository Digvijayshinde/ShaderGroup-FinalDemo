#include"../../Source/Common.h"
#include "../../Source/Globals.h"
#include "../../Renderres/Headers/Renderers.h"


 GLuint vaoSphere;
 GLuint vboSphere;
 GLuint vboSphereElement;

static int numElements;

int Renderers::initializeSphereRenderer() {

	//code

	//variable declarations
	std::vector<vmath::vec3> positions;
	std::vector<vmath::vec2> texcoords;
	std::vector<vmath::vec3> normals;
	std::vector<unsigned int> indices;

	const unsigned int X_SEGMENTS = 128;
	const unsigned int Y_SEGMENTS = 128;

	//code
	for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
	{
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = (float)cos(xSegment * 2.0f * (float)M_PI) * (float)sin(ySegment * (float)M_PI);
			float yPos = (float)cos(ySegment * (float)M_PI);
			float zPos = (float)sin(xSegment * 2.0f * (float)M_PI) * (float)sin(ySegment * (float)M_PI);

			positions.push_back(vmath::vec3(xPos, yPos, zPos));
			texcoords.push_back(vmath::vec2(xSegment, ySegment));
			normals.push_back(vmath::vec3(xPos, yPos, zPos));
		}
	}

	bool oddRow = false;
	for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
	{
		if (!oddRow) // even rows: y == 0, y == 2; and so on
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				indices.push_back(y * (X_SEGMENTS + 1) + x);
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
			}
		}
		else
		{
			for (int x = X_SEGMENTS; x >= 0; --x)
			{
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				indices.push_back(y * (X_SEGMENTS + 1) + x);
			}
		}
		oddRow = !oddRow;
	}

	numElements = (int)indices.size();

	std::vector<float> data;
	for (std::size_t i = 0; i < positions.size(); ++i)
	{
		data.push_back(positions[i][0]);
		data.push_back(positions[i][1]);
		data.push_back(positions[i][2]);

		if (texcoords.size() > 0)
		{
			data.push_back(texcoords[i][0]);
			data.push_back(texcoords[i][1]);
		}
		if (normals.size() > 0)
		{
			data.push_back(normals[i][0]);
			data.push_back(normals[i][1]);
			data.push_back(normals[i][2]);
		}
	}

	int stride = (3 + 2 + 3) * sizeof(float);

	//sphere
	glGenVertexArrays(1, &vaoSphere);
	glBindVertexArray(vaoSphere);

	//vertices
	glGenBuffers(1, &vboSphere);
	glBindBuffer(GL_ARRAY_BUFFER, vboSphere);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

	//vertices
	glVertexAttribPointer(SHADER_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(SHADER_ATTRIBUTE_POSITION);

	//texture
	glVertexAttribPointer(SHADER_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(SHADER_ATTRIBUTE_TEXTURE0);

	//normal
	glVertexAttribPointer(SHADER_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(SHADER_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Color
	glVertexAttrib3f(SHADER_ATTRIBUTE_COLOR,1.0f,1.0f,1.0f);

	//elements
	glGenBuffers(1, &vboSphereElement);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSphereElement);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	glBindVertexArray(0);
	return 0;
}
void Renderers::renderSphere() {

	//code
	//bind vao
	glBindVertexArray(vaoSphere);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSphereElement);
	glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)numElements, GL_UNSIGNED_INT, 0);

	//unbind vao
	glBindVertexArray(0);

}
void Renderers::unitializeSphere() {

	//code
	//destroy vbo
	if (vboSphere)
	{
		glDeleteBuffers(1, &vboSphere);
		vboSphere = 0;
	}

	if (vboSphereElement)
	{
		glDeleteBuffers(1, &vboSphereElement);
		vboSphereElement = 0;
	}

	if (vaoSphere)
	{
		glDeleteVertexArrays(1, &vaoSphere);
		vaoSphere = 0;
	}

}