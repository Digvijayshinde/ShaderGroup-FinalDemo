#pragma once
#include "../../../Source/Globals.h"
#include "../../../Utility/Headers/Ocean/3Dmath.h"


struct OpenGLVertexElement
{
	GLushort	Stream;
	GLushort	Offset;
	GLubyte		Type;			// OpenGLDeclType
	GLubyte		Usage;			// OpenGLDeclUsage
	GLubyte		UsageIndex;
};

struct OpenGLVertexDeclaration
{
	OpenGLVertexElement* Elements;
	GLuint					Stride;
};

struct OpenGLAttributeRange
{
	GLenum		PrimitiveType;	// OpenGLPrimitiveType
	GLuint		AttribId;
	GLuint		IndexStart;
	GLuint		IndexCount;
	GLuint		VertexStart;
	GLuint		VertexCount;
	GLboolean	Enabled;
};

struct OpenGLMaterial
{
	Math::Color	Diffuse;
	Math::Color	Ambient;
	Math::Color	Specular;
	Math::Color	Emissive;
	float		Power;
	GLuint		Texture;
	GLuint		NormalMap;

	OpenGLMaterial();
	~OpenGLMaterial();
};

enum OpenGLMeshFlags
{
	GLMESH_DYNAMIC = 1,
	GLMESH_32BIT = 2
};

class OpenGLMesh
{
	friend bool GLCreateMesh(GLuint, GLuint, GLuint, OpenGLVertexElement*, OpenGLMesh**);

	struct LockedData
	{
		void* ptr;
		GLuint flags;
	};

private:
	Math::AABox					boundingbox;
	OpenGLAttributeRange* subsettable;
	OpenGLMaterial* materials;
	OpenGLVertexDeclaration		vertexdecl;

	GLuint						meshoptions;
	GLuint						numsubsets;
	GLuint						numvertices;
	GLuint						numindices;
	GLuint						vertexbuffer;
	GLuint						indexbuffer;
	GLuint						vertexlayout;

	LockedData					vertexdata_locked;
	LockedData					indexdata_locked;

	OpenGLMesh();

	void Destroy();
	void RecreateVertexLayout();

public:
	~OpenGLMesh();

	bool LockVertexBuffer(GLuint offset, GLuint size, GLuint flags, void** data);
	bool LockIndexBuffer(GLuint offset, GLuint size, GLuint flags, void** data);

	void Draw();
	void DrawInstanced(GLuint numinstances);
	void DrawSubset(GLuint subset, bool bindtextures = false);
	void DrawSubsetInstanced(GLuint subset, GLuint numinstances, bool bindtextures = false);
	void EnableSubset(GLuint subset, bool enable);
	void GenerateTangentFrame();
	void ReorderSubsets(GLuint newindices[]);
	void UnlockVertexBuffer();
	void UnlockIndexBuffer();
	void SetAttributeTable(const OpenGLAttributeRange* table, GLuint size);

	inline void SetBoundingBox(const Math::AABox& box) { boundingbox = box; }
	inline OpenGLAttributeRange* GetAttributeTable() { return subsettable; }
	inline OpenGLMaterial* GetMaterialTable() { return materials; }
	inline const Math::AABox& GetBoundingBox() const { return boundingbox; }
	inline size_t GetNumBytesPerVertex() const { return vertexdecl.Stride; }
	inline GLuint GetNumSubsets() const { return numsubsets; }
	inline GLuint GetNumVertices() const { return numvertices; }
	inline GLuint GetNumIndices() const { return numindices; }
	inline GLuint GetVertexLayout() const { return vertexlayout; }
	inline GLuint GetVertexBuffer() const { return vertexbuffer; }
	inline GLuint GetIndexBuffer() const { return indexbuffer; }
	inline GLenum GetIndexType() const { return ((meshoptions & GLMESH_32BIT) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT); }
};
