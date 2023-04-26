#pragma once

#include <GL\glew.h>
#include <cinder\CinderMath.h>

using namespace Cinder::Math;

class Actor;

struct Vertex
{
	vec3f Position;
	vec3f Normal;
	vec2f Texcoords;
};

class Buffers
{
public:
	Buffers() 
	{
		Destroy();
	}

	~Buffers() 
	{
	}

public:
	void BindBuffers();
	void UnbindBuffer();

	Vertex* OpenVertexBuffer();
	void CloseVertexBuffer();

	void DrawVertices();

	void Destroy();

public:
	/*inline uint32 GetDrawMode() { return DrawMode; }
	inline void SetDrawMode(const uint32 &mode) { DrawMode = mode; }*/

	inline uint32 GetPrimitiveType() { return PrimitiveType; }
	inline void SetPrimitiveType(const int32 &type) { PrimitiveType = type; }

	inline uint32 GetVBO() { return VBO; }
	inline void SetVBO(const uint32 &vbo) { VBO = vbo; }

	inline uint32 GetVAO() { return VAO; }
	inline void SetVAO(const uint32 &vao) { VAO = vao; }

	inline uint32 GetVertexCount() { return VertexCount; }
	inline void SetVertexCount(const uint32 &vertCount) { VertexCount = vertCount; }

	inline float GetLineWidth() { return LineWidth; }
	inline void SetLineWidth(const float &lw) { LineWidth = lw; }

private:
	uint32 VBO;
	uint32 VAO;
	//uint32 DrawMode;
	int32 PrimitiveType;
	uint32 VertexCount;
	float LineWidth;
};

class Mesh
{
public:
	Mesh()
	{}

	~Mesh() 
	{
	}

public:
	inline vec3f GetMinBounds() { return MinBounds; }
	inline vec3f GetMaxBounds() { return MaxBounds; }
	inline vec3f GetSize() { return Size; }
	inline float GetVolume() { return Volume; }

	inline uint32 GetVertexCount() { return MeshBuffers.GetVertexCount(); }
	inline void SetVertexCount(const uint32 &vertCount) { MeshBuffers.SetVertexCount(vertCount); }

	inline Buffers& GetBuffers() { return MeshBuffers; }
	inline void SetBuffers(Buffers &buffers) { MeshBuffers = buffers; }

	void CalculateBounds();
	void CalculateSize();
	void CalculateVolume();

	float CalculateSignedVolume(vec3f &p1, vec3f &p2, vec3f &p3);

	void Destroy();

private:
	vec3f MinBounds;
	vec3f MaxBounds;
	vec3f Size;
	float Volume;

	Buffers MeshBuffers;
};
