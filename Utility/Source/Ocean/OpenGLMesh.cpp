#include "../../../Utility/Headers/Ocean/OpenGLMesh.h"
#include "../../../Utility/Headers/Ocean/OceanHelper.h"
#include <vector>
#include <cassert>

OpenGLMaterial::OpenGLMaterial()
{
	Power = 0.0f;
	Texture = 0;
	NormalMap = 0;
}

OpenGLMaterial::~OpenGLMaterial()
{
	if (Texture != 0)
		glDeleteTextures(1, &Texture);

	if (NormalMap != 0)
		glDeleteTextures(1, &NormalMap);
}

OpenGLMesh::OpenGLMesh()
{
	numsubsets = 0;
	numvertices = 0;
	numindices = 0;
	subsettable = nullptr;
	materials = nullptr;
	vertexbuffer = 0;
	indexbuffer = 0;
	vertexlayout = 0;
	meshoptions = 0;

	vertexdecl.Elements = 0;
	vertexdecl.Stride = 0;

	vertexdata_locked.ptr = 0;
	indexdata_locked.ptr = 0;
}

OpenGLMesh::~OpenGLMesh()
{
	Destroy();
}

void OpenGLMesh::Destroy()
{
	delete[] vertexdecl.Elements;

	GL_SAFE_DELETE_BUFFER(vertexbuffer);
	GL_SAFE_DELETE_BUFFER(indexbuffer);

	if (vertexlayout != 0) 
	{
		glDeleteVertexArrays(1, &vertexlayout);
		vertexlayout = 0;
	}

	delete[] subsettable;
	delete[] materials;

	subsettable = nullptr;
	materials = nullptr;
	numsubsets = 0;
}

void OpenGLMesh::RecreateVertexLayout()
{
	if (vertexlayout != 0)
		glDeleteVertexArrays(1, &vertexlayout);

	glGenVertexArrays(1, &vertexlayout);
	vertexdecl.Stride = 0;

	// calculate stride
	for (int i = 0; i < 16; ++i) 
	{
		OpenGLVertexElement& elem = vertexdecl.Elements[i];

		if (elem.Stream == 0xff)
			break;

		switch (elem.Type) 
		{
		case GLDECLTYPE_GLCOLOR:
		case GLDECLTYPE_FLOAT1:		vertexdecl.Stride += 4;		break;
		case GLDECLTYPE_FLOAT2:		vertexdecl.Stride += 8;		break;
		case GLDECLTYPE_FLOAT3:		vertexdecl.Stride += 12;	break;
		case GLDECLTYPE_FLOAT4:		vertexdecl.Stride += 16;	break;

		default:
			break;
		}
	}

	glBindVertexArray(vertexlayout);
	{
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

		// bind locations
		for (int i = 0; i < 16; ++i) 
		{
			OpenGLVertexElement& elem = vertexdecl.Elements[i];

			if (elem.Stream == 0xff)
				break;

			glEnableVertexAttribArray(elem.Usage);

			switch (elem.Usage) 
			{
			case GLDECLUSAGE_POSITION:
				glVertexAttribPointer(elem.Usage, (elem.Type == GLDECLTYPE_FLOAT4 ? 4 : 3), GL_FLOAT, GL_FALSE, vertexdecl.Stride, (const GLvoid*)elem.Offset);
				break;

			case GLDECLUSAGE_COLOR:
				glVertexAttribPointer(elem.Usage, 4, GL_UNSIGNED_BYTE, GL_TRUE, vertexdecl.Stride, (const GLvoid*)elem.Offset);
				break;

			case GLDECLUSAGE_NORMAL:
				glVertexAttribPointer(elem.Usage, (elem.Type == GLDECLTYPE_FLOAT4 ? 4 : 3), GL_FLOAT, GL_FALSE, vertexdecl.Stride, (const GLvoid*)elem.Offset);
				break;

			case GLDECLUSAGE_TEXCOORD:
				// haaack...
				glVertexAttribPointer(elem.Usage + elem.UsageIndex, (elem.Type + 1), GL_FLOAT, GL_FALSE, vertexdecl.Stride, (const GLvoid*)elem.Offset);
				break;

			case GLDECLUSAGE_TANGENT:
				glVertexAttribPointer(elem.Usage, 3, GL_FLOAT, GL_FALSE, vertexdecl.Stride, (const GLvoid*)elem.Offset);
				break;

			case GLDECLUSAGE_BINORMAL:
				glVertexAttribPointer(elem.Usage, 3, GL_FLOAT, GL_FALSE, vertexdecl.Stride, (const GLvoid*)elem.Offset);
				break;

				// TODO:

			default:
				PrintLog("Unhandled layout element...\n");
				break;
			}
		}
	}
	glBindVertexArray(0);
}

bool OpenGLMesh::LockVertexBuffer(GLuint offset, GLuint size, GLuint flags, void** data)
{
	if (offset >= numvertices * vertexdecl.Stride) {
		(*data) = nullptr;
		return false;
	}

	if (size == 0)
		size = numvertices * vertexdecl.Stride - offset;

	if (flags == 0)
		flags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

	vertexdata_locked.ptr = glMapBufferRange(GL_ARRAY_BUFFER, offset, size, flags);
	vertexdata_locked.flags = flags;

	if (!vertexdata_locked.ptr)
		return false;

	(*data) = vertexdata_locked.ptr;
	return true;
}

bool OpenGLMesh::LockIndexBuffer(GLuint offset, GLuint size, GLuint flags, void** data)
{
	GLuint istride = ((meshoptions & GLMESH_32BIT) ? 4 : 2);

	if (offset >= numindices * istride) 
	{
		(*data) = nullptr;
		return false;
	}

	if (size == 0)
		size = numindices * istride - offset;

	if (flags == 0)
		flags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

	indexdata_locked.ptr = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, offset, size, flags);
	indexdata_locked.flags = flags;

	if (!indexdata_locked.ptr)
		return false;

	(*data) = indexdata_locked.ptr;
	return true;
}

void OpenGLMesh::Draw()
{
	for (GLuint i = 0; i < numsubsets; ++i)
		DrawSubset(i);
}

void OpenGLMesh::DrawInstanced(GLuint numinstances)
{
	for (GLuint i = 0; i < numsubsets; ++i)
		DrawSubsetInstanced(i, numinstances);
}

void OpenGLMesh::DrawSubset(GLuint subset, bool bindtextures)
{
	if (vertexlayout == 0 || numvertices == 0)
		return;

	if (subsettable != nullptr && subset < numsubsets)
	{
		const OpenGLAttributeRange& attr = subsettable[subset];
		const OpenGLMaterial& mat = materials[subset];

		if (!attr.Enabled)
			return;

		GLenum itype = (meshoptions & GLMESH_32BIT) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
		GLuint start = attr.IndexStart * ((meshoptions & GLMESH_32BIT) ? 4 : 2);

		if (bindtextures) 
		{
			if (mat.Texture != 0) 
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mat.Texture);
			}

			if (mat.NormalMap != 0) 
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, mat.NormalMap);
			}
		}

		glBindVertexArray(vertexlayout);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

		if (attr.IndexCount == 0) 
		{
			glDrawArrays(attr.PrimitiveType, attr.VertexStart, attr.VertexCount);
		}
		else 
		{
			if (attr.VertexCount == 0)
				glDrawElements(attr.PrimitiveType, attr.IndexCount, itype, (char*)0 + start);
			else
				glDrawRangeElements(attr.PrimitiveType, attr.VertexStart, attr.VertexStart + attr.VertexCount - 1, attr.IndexCount, itype, (char*)0 + start);
		}
	}
}

void OpenGLMesh::DrawSubsetInstanced(GLuint subset, GLuint numinstances, bool bindtextures)
{
	// NOTE: use SSBO, dummy... (or modify this class)

	if (vertexlayout == 0 || numvertices == 0 || numinstances == 0)
		return;

	if (subsettable != nullptr && subset < numsubsets) 
	{
		const OpenGLAttributeRange& attr = subsettable[subset];
		const OpenGLMaterial& mat = materials[subset];

		if (!attr.Enabled)
			return;

		GLenum itype = (meshoptions & GLMESH_32BIT) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
		GLuint start = attr.IndexStart * ((meshoptions & GLMESH_32BIT) ? 4 : 2);

		if (bindtextures) 
		{
			if (mat.Texture != 0)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mat.Texture);
			}

			if (mat.NormalMap != 0)
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, mat.NormalMap);
			}
		}

		glBindVertexArray(vertexlayout);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

		if (attr.IndexCount == 0)
			glDrawArraysInstanced(attr.PrimitiveType, attr.VertexStart, attr.VertexCount, numinstances);
		else
			glDrawElementsInstanced(attr.PrimitiveType, attr.IndexCount, itype, (char*)0 + start, numinstances);
	}
}

void OpenGLMesh::EnableSubset(GLuint subset, bool enable)
{
	if (subsettable != nullptr && subset < numsubsets)
		subsettable[subset].Enabled = enable;
}

void OpenGLMesh::GenerateTangentFrame()
{
	GL_ASSERT(vertexdecl.Stride == sizeof(GeometryUtils::CommonVertex));
	GL_ASSERT(vertexbuffer != 0);

	GeometryUtils::CommonVertex* oldvdata = 0;
	GeometryUtils::TBNVertex* newvdata = 0;
	void* idata = 0;
	GLuint							newbuffer = 0;
	uint32_t						i1, i2, i3;
	bool							is32bit = ((meshoptions & GLMESH_32BIT) == GLMESH_32BIT);

	GL_ASSERT(LockVertexBuffer(0, 0, GLLOCK_READONLY, (void**)&oldvdata));
	GL_ASSERT(LockIndexBuffer(0, 0, GLLOCK_READONLY, (void**)&idata));

	newvdata = new GeometryUtils::TBNVertex[numvertices];

	for (GLuint i = 0; i < numsubsets; ++i) {
		const OpenGLAttributeRange& subset = subsettable[i];
		GL_ASSERT(subset.IndexCount > 0);

		GeometryUtils::CommonVertex* oldsubsetdata = (oldvdata + subset.VertexStart);
		GeometryUtils::TBNVertex* newsubsetdata = (newvdata + subset.VertexStart);
		void* subsetidata = ((uint16_t*)idata + subset.IndexStart);

		if (is32bit)
			subsetidata = ((uint32_t*)idata + subset.IndexStart);

		// initialize new data
		for (uint32_t j = 0; j < subset.VertexCount; ++j) {
			GeometryUtils::CommonVertex& oldvert = oldsubsetdata[j];
			GeometryUtils::TBNVertex& newvert = newsubsetdata[j];

			newvert.x = oldvert.x;
			newvert.y = oldvert.y;
			newvert.z = oldvert.z;

			newvert.nx = oldvert.nx;
			newvert.ny = oldvert.ny;
			newvert.nz = oldvert.nz;

			newvert.u = oldvert.u;
			newvert.v = oldvert.v;

			newvert.tx = newvert.bx = 0;
			newvert.ty = newvert.by = 0;
			newvert.tz = newvert.bz = 0;
		}

		for (uint32_t j = 0; j < subset.IndexCount; j += 3) {
			if (is32bit) {
				i1 = *((uint32_t*)subsetidata + j + 0) - subset.VertexStart;
				i2 = *((uint32_t*)subsetidata + j + 1) - subset.VertexStart;
				i3 = *((uint32_t*)subsetidata + j + 2) - subset.VertexStart;
			}
			else {
				i1 = *((uint16_t*)subsetidata + j + 0) - subset.VertexStart;
				i2 = *((uint16_t*)subsetidata + j + 1) - subset.VertexStart;
				i3 = *((uint16_t*)subsetidata + j + 2) - subset.VertexStart;
			}

			GeometryUtils::AccumulateTangentFrame(newsubsetdata, i1, i2, i3);
		}

		for (uint32_t j = 0; j < subset.VertexCount; ++j) {
			GeometryUtils::OrthogonalizeTangentFrame(newsubsetdata[j]);
		}
	}

	UnlockIndexBuffer();
	UnlockVertexBuffer();

	glDeleteBuffers(1, &vertexbuffer);
	glGenBuffers(1, &vertexbuffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, numvertices * sizeof(GeometryUtils::TBNVertex), newvdata, ((meshoptions & GLMESH_DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete[] newvdata;

	OpenGLVertexElement* olddecl = vertexdecl.Elements;
	int numdeclelems = 0;

	for (size_t i = 0; i < 16; ++i) {
		const OpenGLVertexElement& elem = vertexdecl.Elements[i];

		if (elem.Stream == 0xff)
			break;

		++numdeclelems;
	}

	vertexdecl.Elements = new OpenGLVertexElement[numdeclelems + 3];
	memcpy(vertexdecl.Elements, olddecl, numdeclelems * sizeof(OpenGLVertexElement));

	vertexdecl.Elements[numdeclelems + 0].Stream = 0;
	vertexdecl.Elements[numdeclelems + 0].Offset = 32;
	vertexdecl.Elements[numdeclelems + 0].Type = GLDECLTYPE_FLOAT3;
	vertexdecl.Elements[numdeclelems + 0].Usage = GLDECLUSAGE_TANGENT;
	vertexdecl.Elements[numdeclelems + 0].UsageIndex = 0;

	vertexdecl.Elements[numdeclelems + 1].Stream = 0;
	vertexdecl.Elements[numdeclelems + 1].Offset = 44;
	vertexdecl.Elements[numdeclelems + 1].Type = GLDECLTYPE_FLOAT3;
	vertexdecl.Elements[numdeclelems + 1].Usage = GLDECLUSAGE_BINORMAL;
	vertexdecl.Elements[numdeclelems + 1].UsageIndex = 0;

	vertexdecl.Elements[numdeclelems + 2].Stream = 0xff;

	delete[] olddecl;
	RecreateVertexLayout();

	GL_ASSERT(vertexdecl.Stride == sizeof(GeometryUtils::TBNVertex));
}

void OpenGLMesh::ReorderSubsets(GLuint newindices[])
{
	OpenGLAttributeRange tmp;

	for (GLuint i = 0; i < numsubsets; ++i) {
		tmp = subsettable[i];
		subsettable[i] = subsettable[newindices[i]];
		subsettable[newindices[i]] = tmp;

		for (GLuint j = i; j < numsubsets; ++j) {
			if (newindices[j] == i) {
				newindices[j] = newindices[i];
				break;
			}
		}
	}
}

void OpenGLMesh::UnlockVertexBuffer()
{
	if (vertexdata_locked.ptr != nullptr && vertexbuffer != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glUnmapBuffer(GL_ARRAY_BUFFER);

		vertexdata_locked.ptr = nullptr;
	}
}

void OpenGLMesh::UnlockIndexBuffer()
{
	if (indexdata_locked.ptr != nullptr && indexbuffer != 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

		indexdata_locked.ptr = nullptr;
	}
}

void OpenGLMesh::SetAttributeTable(const OpenGLAttributeRange* table, GLuint size)
{
	delete[] subsettable;

	subsettable = new OpenGLAttributeRange[size];
	memcpy(subsettable, table, size * sizeof(OpenGLAttributeRange));

	numsubsets = size;
}
