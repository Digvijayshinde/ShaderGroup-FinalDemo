#pragma once
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


#include <assert.h>


#include "Mesh.h"

struct BoneInfo
{
    /*id is index in finalBoneMatrices*/
    int id;

    /*offset matrix transforms vertex from model space to bone space*/
    mat4 offset;
};

inline mat4 ConvertMatrixToMat4Format(const aiMatrix4x4& from)
{
    mat4 to;
    //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

class Model
{
public:
    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;

    Model(bool gamma = false) : gammaCorrection(gamma)
    {

    }

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    auto& GetBoneInfoMap()
    { 
        return m_BoneInfoMap; 
    }
    
    int& GetBoneCount()
    {
        return m_BoneCounter; 
    }



    // draws the model, and thus all its meshes
    void Draw(GLuint shaderObject, int instanceCount = 1, BOOL enableBlend = FALSE)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shaderObject, instanceCount, enableBlend);
    }

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            PrintLog("ERROR::ASSIMP:: %s\n",importer.GetErrorString());
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

private:
    std::map<string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    void SetVertexBoneDataToDefault(Vertex& vertex)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            vertex.m_BoneIDs[i] = -1;
            vertex.m_Weights[i] = 0.0f;
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        MaterialColor materialColor;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            SetVertexBoneDataToDefault(vertex);
            vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector[0] = mesh->mVertices[i].x;
            vector[1] = mesh->mVertices[i].y;
            vector[2] = mesh->mVertices[i].z;
            vertex.Position = vector;

            // normals
            if (mesh->HasNormals())
            {
                vector[0] = mesh->mNormals[i].x;
                vector[1] = mesh->mNormals[i].y;
                vector[2] = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec[0] = mesh->mTextureCoords[0][i].x;
                vec[1] = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector[0] = mesh->mTangents[i].x;
                vector[1] = mesh->mTangents[i].y;
                vector[2] = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector[0] = mesh->mBitangents[i].x;
                vector[1] = mesh->mBitangents[i].y;
                vector[2] = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        //colors -- for colors refer: https://github.com/assimp/assimp/blob/master/samples/SimpleOpenGL/Sample_SimpleOpenGL.c
        C_STRUCT aiColor4D diffuse;
        C_STRUCT aiColor4D specular;
        C_STRUCT aiColor4D ambient;
        C_STRUCT aiColor4D emission;
        ai_real shininess, strength;

        if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
        {
            materialColor.diffuse[0] = diffuse.r;
            materialColor.diffuse[1] = diffuse.g;
            materialColor.diffuse[2] = diffuse.b;
            materialColor.diffuse[3] = diffuse.a;
        }

        if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular))
        {
            materialColor.specular[0] = specular.r;
            materialColor.specular[1] = specular.g;
            materialColor.specular[2] = specular.b;
            materialColor.specular[3] = specular.a;
        }

        if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient))
        {
            materialColor.ambient[0] = ambient.r;
            materialColor.ambient[1] = ambient.g;
            materialColor.ambient[2] = ambient.b;
            materialColor.ambient[3] = ambient.a;
        }

        if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &emission))
        {
            materialColor.emission[0] = emission.r;
            materialColor.emission[1] = emission.g;
            materialColor.emission[2] = emission.b;
            materialColor.emission[3] = emission.a;
        }

        unsigned int max = 1;
        int ret1 = aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &max);
        if (ret1 == AI_SUCCESS) 
        {
            max = 1;
            int ret2 = aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
            if (ret2 == AI_SUCCESS)
            {
                //glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
                shininess = shininess * strength;
                materialColor.shininess = (float)shininess;
            }
            else
            {
                materialColor.shininess = (float)shininess;
            }
        }
        else 
        {
            materialColor.shininess = 0.0f;
            materialColor.specular[0] = 0.0f;
            materialColor.specular[1] = 0.0f;
            materialColor.specular[2] = 0.0f;
            materialColor.specular[3] = 0.0f;
        }

        ExtractBoneWeightForVertices(vertices, mesh, scene);

        for (int i = 0; i < vertices.size(); i++)
        {
            float totalWeight = 0.0f;
            for (int m = 0; m < MAX_BONE_INFLUENCE; m++)
            {
                totalWeight += vertices[i].m_Weights[m];

            }
            if (totalWeight > 0.0f)
            {
                //the total addition of wight should be 1
                for (int m = 0; m < MAX_BONE_INFLUENCE; m++)
                {
                    vertices[i].m_Weights[m] /= totalWeight;
                }
            }
        }

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures, materialColor);
    }

    void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            if (vertex.m_BoneIDs[i] < 0)
            {
                vertex.m_Weights[i] = weight;
                vertex.m_BoneIDs[i] = boneID;
                break;
            }
        }
    }

    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
    {
        auto& boneInfoMap = m_BoneInfoMap;
        int& boneCount = m_BoneCounter;

        for (int boneIndex = 0; boneIndex < (int)mesh->mNumBones; ++boneIndex)
        {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id = boneCount;
                newBoneInfo.offset = ConvertMatrixToMat4Format(mesh->mBones[boneIndex]->mOffsetMatrix);
                boneInfoMap[boneName] = newBoneInfo;
                boneID = boneCount;
                boneCount++;
            }
            else
            {
                boneID = boneInfoMap[boneName].id;
            }

            assert(boneID != -1);
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= vertices.size());
                SetVertexBoneData(vertices[vertexId], boneID, weight);
            }
        }
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        TextureManager texturemanager;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                //texture.id = LoadTextureFromFileWithSTB(str.C_Str(), this->directory);

                texturemanager.storeTextureFromFile(this->directory, str.C_Str());
                string textureFileNameWithExtension=str.C_Str();
                int pos = textureFileNameWithExtension.find('.');
                std::string fileNameWithoutExtension = textureFileNameWithExtension.substr(0, pos);
                texture.id = texturemanager.getTexture(fileNameWithoutExtension);

                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
            }
        }
        return textures;
    }
};
