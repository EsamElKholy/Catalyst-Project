#pragma once

#include <cinder\CinderMath.h>
#include "Utilities.h"

using namespace Cinder::Math;

struct Vertex;
class Buffers;
class Mesh;

class MeshFactory
{
public:
	~MeshFactory() {}

private:
	MeshFactory() {}
	MeshFactory(const MeshFactory &ms) = default;
	MeshFactory& operator=(const MeshFactory &ms) = default;

public:
	static Mesh CreateMesh(Vertex *vertices, const uint32 &vertCount, bool calcNormals, bool calcTexcoords, vec2f &ts = vec2f());
	static Mesh CreateMeshSTL(const std::string &path, bool withTextures, vec2f &ts = vec2f());
	static Mesh CreateMeshOBJ(const std::string &path, bool withTextures, vec2f &ts = vec2f());

	static Mesh CreateQuad(vec3f &size, char *plane, char *direction, bool withTextures, bool reload = false, Mesh *mesh = NULL, vec2f &ts = vec2f());
	static Mesh CreateSprite(vec3f &size, bool reload = false, Mesh *mesh = NULL, vec2f &ts = vec2f());
	static Mesh CreateCircle(vec2f &size, int triangles, bool withTextures, bool reload = false, Mesh *mesh = NULL, vec2f &ts = vec2f());
	static Mesh CreateCuboid(vec3f &size, bool withTextures, bool reload = false, Mesh *mesh = NULL, vec2f &ts = vec2f());
	static Mesh CreateWiredCuboid(vec3f &size, bool reload = false, Mesh *mesh = NULL);
	static Mesh CreateGrid(float horizontal, float vertical, float spacing, char *plane, bool reload = false, Mesh *mesh = NULL);
	static Mesh CreateHexagonGrid(vec3f bottomLeft, float r, float nh, float nv, float lw, char *direction);
	static Mesh CreateTetrahedronSphere(float rad, int recursionLevel, bool withTextures, bool reload = false, Mesh *mesh = NULL, vec2f &ts = vec2f());
	static Mesh CreateSphere(vec3f rad, int recursionLevel, bool withTextures, bool reload = false, Mesh *mesh = NULL, vec2f &ts = vec2f());
	static Mesh CreateCylinder(float h, vec3f r1, vec3f r2, int m, bool withTextures, bool reload = false, Mesh *mesh = NULL, vec2f &ts = vec2f());
	static std::pair<vec3f, vec3f> MeshFactory::CalculateBounds(Vertex *vertices, uint32 vertCount);
	
private:
	static void Triangle(std::vector<Vertex> &vertices, vec3f &a, vec3f &b, vec3f &c);
	static void DivideTriangle(std::vector<Vertex> &vertices, vec3f &a, vec3f &b, vec3f &c, int iter, float rad);
	static Buffers CreateVertexBuffer(Vertex *vertices, const uint32 &vertCount);
	static void CalculateNormals(Vertex *vertices, const uint32 &vertCount);
	static void CalculateTextureCoordinates(Vertex *vertices, const uint32 &vertCount, vec3f minBounds, vec3f maxBounds, vec2f &ts = vec2f());
	static bool FacingThisWay(vec3f &v, vec3f &dir, float &maxDot, int facing, int &ret);
	static int FacingDirection(vec3f &v);
};

struct TriangleIndices
{
	int V1;
	int V2;
	int V3;
};

class IcoSphereCreator
{
public:
	IcoSphereCreator(vec3f rad = vec3f(10, 10, 10))
		: Raduis(rad)
	{}
	~IcoSphereCreator() {}

public:
	std::vector<Vertex>& Create(int recursionLevel);

private:
	int AddVertex(Vertex v);

	// return index of point in the middle of p1 and p2
	int GetMiddlePoint(int p1, int p2);

private:
	std::vector<Vertex> _Vertices;
	std::vector<Vertex> Vertices;
	int Index;
	std::map<uint64, int32> MiddlePointIndexCache;
	vec3f Raduis;
};
