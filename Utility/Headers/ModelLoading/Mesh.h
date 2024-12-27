#pragma once
#include "../../../Source/Common.h"
#include <string>
#define MAX_BONE_INFLUENCE 4

typedef struct Vertex
{
	// position
	vec3 Position;
	// normal
	vec3 Normal;
	// texCoords
	vec2 TexCoords;
	// tangent
	vec3 Tangent;
	// bitangent
	vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
} Vertex;

typedef struct Texture
{
	unsigned int id;
	string type;
	string path;
} Texture;

typedef struct MaterialColor
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emission;
    float shininess;
}MaterialColor;

class Mesh
{
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    MaterialColor        materialColor;
    unsigned int VAO;

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, MaterialColor materialColor)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->materialColor = materialColor;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
    void Draw(GLuint& shaderObject, int instanceCount, BOOL enableBlend)
    {
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        int textureFound = 0;

        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to string
            else if (name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to string
            else if (name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to string

            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shaderObject, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
            textureFound = 1;
        }
        //If model has no tetxure it should display with black color
        if(textureFound ==0)
            glBindTexture(GL_TEXTURE_2D, 0);


        glUniform4fv(glGetUniformLocation(shaderObject, "u_Ka"), 1, materialColor.ambient);
        glUniform4fv(glGetUniformLocation(shaderObject, "u_Kd"), 1, materialColor.diffuse);
        glUniform4fv(glGetUniformLocation(shaderObject, "u_Ks"), 1, materialColor.specular);
        glUniform4fv(glGetUniformLocation(shaderObject, "u_Ke"), 1, materialColor.emission);
        glUniform1f(glGetUniformLocation(shaderObject, "u_materialShininess"), materialColor.shininess);

        if (enableBlend == TRUE)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        // draw mesh
        if (instanceCount > 1)
        {
            glBindVertexArray(VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0, instanceCount);
            glBindVertexArray(0);
        }
        else
        {
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
        
        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
  

        if (enableBlend == TRUE)
        {
            glDisable(GL_BLEND);
        }
    }

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(SHADER_ATTRIBUTE_POSITION);
        glVertexAttribPointer(SHADER_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(SHADER_ATTRIBUTE_NORMAL);
        glVertexAttribPointer(SHADER_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(SHADER_ATTRIBUTE_TEXTURE0);
        glVertexAttribPointer(SHADER_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(SHADER_ATTRIBUTE_TANGENT);
        glVertexAttribPointer(SHADER_ATTRIBUTE_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(SHADER_ATTRIBUTE_BITANGENT);
        glVertexAttribPointer(SHADER_ATTRIBUTE_BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        // ids
        glEnableVertexAttribArray(SHADER_ATTRIBUTE_BONEIDS);
        glVertexAttribIPointer(SHADER_ATTRIBUTE_BONEIDS, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

        // weights
        glEnableVertexAttribArray(SHADER_ATTRIBUTE_WEIGHT);
        glVertexAttribPointer(SHADER_ATTRIBUTE_WEIGHT, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        glBindVertexArray(0);
    }
};
