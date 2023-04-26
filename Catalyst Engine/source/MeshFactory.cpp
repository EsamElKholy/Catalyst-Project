#include <MeshFactory.h>
#include <Mesh.h>
#include <OBJ_Loader.h>

Mesh MeshFactory::CreateMesh(Vertex *vertices, const uint32 &vertCount, bool calcNormals, bool calcTexCoords, vec2f &ts)
{
	if (calcNormals)
	{
		CalculateNormals(vertices, vertCount);
	}

	Mesh res;
	
	Buffers b = CreateVertexBuffer(vertices, vertCount);

	res.SetBuffers(b);

	res.CalculateBounds();
	res.CalculateSize();
	res.CalculateVolume();

	if (calcTexCoords)
	{
		Vertex *v = res.GetBuffers().OpenVertexBuffer();

		CalculateTextureCoordinates(v, vertCount, res.GetMinBounds(), res.GetMaxBounds(), ts);

		res.GetBuffers().CloseVertexBuffer();
	}

	return res;
}

Buffers MeshFactory::CreateVertexBuffer(Vertex *vertices, const uint32 &vertCount)
{
	Buffers buffers;

	uint32 vao;
	uint32 vbo;
	
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	uint32 bufferSize = sizeof(Vertex) * vertCount;

	glBufferData(GL_ARRAY_BUFFER, bufferSize, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void *)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void *)(sizeof(vec3f)));
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (void *)(sizeof(vec3f) + sizeof(vec3f)));

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	buffers.SetVAO(vao);
	
	buffers.SetVBO(vbo);

	buffers.SetVertexCount(vertCount);

	return buffers;
}

void MeshFactory::CalculateNormals(Vertex * vertices, const uint32 &vertCount)
{
	for (unsigned int i = 0; i < vertCount; i += 3)
	{
		vec3f v1 = vertices[i + 1].Position - vertices[i + 0].Position;
		vec3f v2 = vertices[i + 2].Position - vertices[i + 0].Position;

		vec3f normal = Vec3::Normalize(Vec3::Cross(v1, v2));

		vertices[i + 0].Normal += normal;
		vertices[i + 1].Normal += normal;
		vertices[i + 2].Normal += normal;
	}

	for (unsigned int i = 0; i < vertCount; i++)
	{
		vertices[i].Normal = Vec3::Normalize(vertices[i].Normal);
	}
}

void MeshFactory::CalculateTextureCoordinates(Vertex * vertices, const uint32 & vertCount, vec3f minBounds, vec3f maxBounds, vec2f &ts)
{
	float kx = 1.0f / (maxBounds.X - minBounds.X);
	float ky = 1.0f / (maxBounds.Y - minBounds.Y);
	float kz = 1.0f / (maxBounds.Z - minBounds.Z);

	for (uint32 i = 0; i < vertCount; i += 3)
	{		
		int i0 = i;
		int i1 = i + 1;
		int i2 = i + 2;

		vec3f p0 = vertices[i0].Position;
		vec3f p1 = vertices[i1].Position;
		vec3f p2 = vertices[i2].Position;

		vec3f side1 = p1 - p0;
		vec3f side2 = p2 - p0;

		vec3f dir = Vec3::Cross(side1, side2);

		int facing = FacingDirection(dir);

		float x = ts.X != 0.0f ? ts.X : 1.0f;
		float y = ts.Y != 0.0f ? ts.Y : 1.0f;

		if (facing == 0)
		{
			vertices[i0].Texcoords.X = (vertices[i0].Position.Z - minBounds.Z) * kz * x;
			vertices[i0].Texcoords.Y = (vertices[i0].Position.Y - minBounds.Y) * ky * y;

			vertices[i1].Texcoords.X = (vertices[i1].Position.Z - minBounds.Z) * kz * x;
			vertices[i1].Texcoords.Y = (vertices[i1].Position.Y - minBounds.Y) * ky * y;
																					
			vertices[i2].Texcoords.X = (vertices[i2].Position.Z - minBounds.Z) * kz * x;
			vertices[i2].Texcoords.Y = (vertices[i2].Position.Y - minBounds.Y) * ky * y;
		}
		else if (facing == 1)
		{
			vertices[i0].Texcoords.X = (vertices[i0].Position.X - minBounds.X) * kx * x;
			vertices[i0].Texcoords.Y = (vertices[i0].Position.Z - minBounds.Z) * kz * y;
																					
			vertices[i1].Texcoords.X = (vertices[i1].Position.X - minBounds.X) * kx * x;
			vertices[i1].Texcoords.Y = (vertices[i1].Position.Z - minBounds.Z) * kz * y;
																					
			vertices[i2].Texcoords.X = (vertices[i2].Position.X - minBounds.X) * kx * x;
			vertices[i2].Texcoords.Y = (vertices[i2].Position.Z - minBounds.Z) * kz * y;
		}
		else
		{
			vertices[i0].Texcoords.X = (vertices[i0].Position.X - minBounds.X) * kx * x;
			vertices[i0].Texcoords.Y = (vertices[i0].Position.Y - minBounds.Y) * ky * y;
																					
			vertices[i1].Texcoords.X = (vertices[i1].Position.X - minBounds.X) * kx * x;
			vertices[i1].Texcoords.Y = (vertices[i1].Position.Y - minBounds.Y) * ky * y;
																					
			vertices[i2].Texcoords.X = (vertices[i2].Position.X - minBounds.X) * kx * x;
			vertices[i2].Texcoords.Y = (vertices[i2].Position.Y - minBounds.Y) * ky * y;
		}
	}
}

bool MeshFactory::FacingThisWay(vec3f & v, vec3f & dir, float & maxDot, int facing, int & ret)
{
	float t = Vec3::Dot(v, dir);

	if (t > maxDot)
	{
		ret = facing;

		maxDot = t;

		return true;
	}

	return false;
}

int MeshFactory::FacingDirection(vec3f & v)
{
	float maxDot = 0;
	int ret = 0;

	if (!FacingThisWay(v, vec3f(1, 0, 0), maxDot, 0, ret))
	{
		FacingThisWay(v, vec3f(-1, 0, 0), maxDot, 0, ret);
	}

	if (!FacingThisWay(v, vec3f(0, 1, 0), maxDot, 1, ret))
	{
		FacingThisWay(v, vec3f(0, -1, 0), maxDot, 1, ret);
	}

	if (!FacingThisWay(v, vec3f(0, 0, 1), maxDot, 2, ret))
	{
		FacingThisWay(v, vec3f(0, 0, -1), maxDot, 2, ret);
	}

	return ret;
}

std::pair<vec3f, vec3f> MeshFactory::CalculateBounds(Vertex *vertices, uint32 vertCount)
{
	vec3f min(999999999.0f, 999999999.0f, 999999999.0f);
	vec3f max(-999999999.0f, -999999999.0f, -999999999.0f);

	for (uint32 i = 0; i < vertCount; i++)
	{
		vec3f pos = vertices[i].Position;

		if (pos.X < min.X)
		{
			min.X = pos.X;
		}

		if (pos.Y < min.Y)
		{
			min.Y = pos.Y;
		}

		if (pos.Z < min.Z)
		{
			min.Z = pos.Z;
		}

		if (pos.X > max.X)
		{
			max.X = pos.X;
		}

		if (pos.Y > max.Y)
		{
			max.Y = pos.Y;
		}

		if (pos.Z > max.Z)
		{
			max.Z = pos.Z;
		}
	}

	return std::pair<vec3f, vec3f>(min, max);
}

Mesh MeshFactory::CreateMeshSTL(const std::string &path, bool withTextures, vec2f &ts)
{
	STLLoader loader;

	loader.load(path);

	Mesh res = CreateMesh(&loader.Vertices[0], loader.Vertices.size(), true, withTextures, ts);
	res.GetBuffers().SetPrimitiveType(GL_TRIANGLES);

	return res;
}

Mesh MeshFactory::CreateMeshOBJ(const std::string &path, bool withTextures, vec2f &ts)
{
	objl::Loader loader;

	loader.LoadFile(path);
	
	std::vector<Vertex> verts; 
	
	for (int i = 0; i < loader.LoadedIndices.size(); i++)
	{
		Vertex v;
		
		objl::Vector3 pos = loader.LoadedVertices[loader.LoadedIndices[i]].Position;
		objl::Vector3 norm = loader.LoadedVertices[loader.LoadedIndices[i]].Normal;
		objl::Vector2 tex = loader.LoadedVertices[loader.LoadedIndices[i]].TextureCoordinate;

		v.Position = vec3f(pos.X, pos.Y, pos.Z);
		v.Normal = vec3f(norm.X, norm.Y, norm.Z);
		v.Texcoords = vec2f(tex.X, tex.Y);

		verts.push_back(v);
	}

	Mesh res = CreateMesh(&verts[0], verts.size(), false, false, ts);
	res.GetBuffers().SetPrimitiveType(GL_TRIANGLES);
	
	return res;
}

Mesh MeshFactory::CreateQuad(vec3f &size, char *plane, char *direction, bool withTextures, bool reload, Mesh *mesh, vec2f &ts)
{
	Vertex temp[4];

	Vertex vertices[6];

	if (plane == "xy" || plane == "yx" || plane == "XY" || plane == "YX")
	{
		temp[0] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f), vec3f(), vec2f(0, 0) };
		temp[1] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f), vec3f(), vec2f(1, 0) };
		temp[2] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f), vec3f(), vec2f(1, 1) };
		temp[3] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f), vec3f(), vec2f(0, 1) };
	}
	else if (plane == "xz" || plane == "zx" || plane == "XZ" || plane == "ZX")
	{
		temp[0] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f, 0.0f + (size.Z / 2.0f)), vec3f(), vec2f(0, 0) };
		temp[1] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f, 0.0f + (size.Z / 2.0f)), vec3f(), vec2f(1, 0) };
		temp[2] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f, 0.0f - (size.Z / 2.0f)), vec3f(), vec2f(1, 1) };
		temp[3] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f, 0.0f - (size.Z / 2.0f)), vec3f(), vec2f(0, 1) };
	}
	else if (plane == "zy" || plane == "yz" || plane == "ZY" || plane == "YZ")
	{
		temp[0] = Vertex{ vec3f(0.0f, 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f(), vec2f(0, 0) };
		temp[1] = Vertex{ vec3f(0.0f, 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f(), vec2f(1, 0) };
		temp[2] = Vertex{ vec3f(0.0f, 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f(), vec2f(1, 1) };
		temp[3] = Vertex{ vec3f(0.0f, 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f(), vec2f(0, 1) };
	}
	else
	{
		temp[0] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f), vec3f(), vec2f(0, 0) };
		temp[1] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f), vec3f(), vec2f(1, 0) };
		temp[2] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f), vec3f(), vec2f(1, 1) };
		temp[3] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f), vec3f(), vec2f(0, 1) };
	}

	if (direction == "front")
	{
		vertices[0] = temp[0];
		vertices[1] = temp[1];
		vertices[2] = temp[2];
					  				  
		vertices[3] = temp[0];
		vertices[4] = temp[2];
		vertices[5] = temp[3];
	}
	else
	{
		vertices[0] = temp[2];
		vertices[1] = temp[1];
		vertices[2] = temp[0];

		vertices[3] = temp[3];
		vertices[4] = temp[2];
		vertices[5] = temp[0];
	}

	Mesh res;

	if (reload)
	{
		if (mesh)
		{
			Vertex *verts = mesh->GetBuffers().OpenVertexBuffer();

			for (uint32 i = 0; i < 6; i++)
			{
				verts[i].Position = vertices[i].Position;
			}

			CalculateNormals(vertices, 6);

			mesh->CalculateBounds();
			mesh->CalculateSize();

			if (withTextures)
			{
				CalculateTextureCoordinates(vertices, 6, mesh->GetMinBounds(), mesh->GetMaxBounds(), ts);
			}
		}
	}
	else
	{
		res = CreateMesh(vertices, 6, true, withTextures, ts);

		res.GetBuffers().SetPrimitiveType(GL_TRIANGLES);
	}

	return res;
}

Mesh MeshFactory::CreateSprite(vec3f & size, bool reload, Mesh * mesh, vec2f & ts)
{
	Mesh sprite;

	sprite = CreateQuad(size, "XY", "front", true);

	return sprite;
}

Mesh MeshFactory::CreateCircle(vec2f & size, int triangles, bool withTextures, bool reload, Mesh * mesh, vec2f & ts)
{
	std::vector<Vertex> vertices;
	std::vector<uint32> idices;

	float startX = 0;
	float startY = 0;

	Vertex pos = Vertex{ vec3f(startX, startY, 0.0f), vec3f(), vec2f(0, 0) };
	vertices.push_back(pos);

	for (float i = 0; i <= triangles; i++)
	{
		float x = size.X * cosf(i * 2.0f * PI / triangles);
		float y = size.Y * sinf(i * 2.0f * PI / triangles);

		Vertex v = Vertex{ vec3f(x, y, 0.0f), vec3f(), vec2f(0, 0) };
		vertices.push_back(v);
	}

	Mesh res;

	if (reload)
	{
		if (mesh)
		{
			Vertex *verts = mesh->GetBuffers().OpenVertexBuffer();

			for (uint32 i = 0; i < 48; i++)
			{
				verts[i].Position = vertices[i].Position;
			}

			CalculateNormals(&vertices[0], vertices.size());

			mesh->CalculateBounds();
			mesh->CalculateSize();
		}
	}
	else
	{
		res = CreateMesh(&vertices[0], vertices.size(), false, false, vec2f());

		res.GetBuffers().SetPrimitiveType(GL_TRIANGLE_FAN);
	}

	return res;
}

Mesh MeshFactory::CreateCuboid(vec3f &size, bool withTextures, bool reload, Mesh *mesh, vec2f &ts)
{
	Vertex vertices[36];

	// Face: UP 
	vertices[0] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[1] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[2] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	
	vertices[3] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[4] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[5] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
																							
	// Face: DOWN 
	vertices[6] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[7] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[8] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	
	vertices[9] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f()  };
	vertices[10] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[11] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };

	// Face: BACK 
	vertices[12] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[13] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[14] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	
	vertices[15] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[16] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[17] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };

	// Face: FRONT 
	vertices[18] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[19] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[20] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };

	vertices[21] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[22] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[23] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };

	// Face: LEFT 
	vertices[24] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[25] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[26] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
			 
	vertices[27] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[28] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[29] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };

	// Face: RIGHT
	vertices[30] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[31] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[32] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
			 
	vertices[33] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[34] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[35] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	
	Mesh res;

	if (reload)
	{
		if (mesh)
		{
			Vertex *verts = mesh->GetBuffers().OpenVertexBuffer();

			for (uint32 i = 0; i < 36; i++)
			{
				verts[i].Position = vertices[i].Position;
			}

			CalculateNormals(vertices, 36);

			mesh->CalculateBounds();
			mesh->CalculateSize();
			
			if (withTextures)
			{
				CalculateTextureCoordinates(vertices, 36, mesh->GetMinBounds(), mesh->GetMaxBounds(), ts);
			}
		}
	}
	else
	{
		res = CreateMesh(vertices, 36, true, withTextures, ts);

		res.GetBuffers().SetPrimitiveType(GL_TRIANGLES);
	}

	return res;
}

Mesh MeshFactory::CreateWiredCuboid(vec3f &size, bool reload, Mesh *mesh)
{
	Vertex vertices[48];

	// Face: UP 
	vertices[0] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[1] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	
	vertices[2] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[3] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	
	vertices[4] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[5] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };

	vertices[6] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[7] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };

	// Face: DOWN 
	vertices[8] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[9] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	
	vertices[10] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[11] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	
	vertices[12] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[13] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	
	vertices[14] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[15] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };

	// Face: BACK 
	vertices[16] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[17] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	
	vertices[18] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[19] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	
	vertices[20] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[21] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	
	vertices[22] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[23] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };

	// Face: FRONT 
	vertices[24] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[25] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	
	vertices[26] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[27] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };

	vertices[28] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[29] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	
	vertices[30] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[31] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };

	// Face: LEFT 
	vertices[32] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[33] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	
	vertices[34] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[35] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	
	vertices[36] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[37] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };

	vertices[38] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[39] = Vertex{ vec3f(0.0f - (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };

	// Face: RIGHT
	vertices[40] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[41] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	
	vertices[42] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[43] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	
	vertices[44] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f - (size.Z / 2.0f)), vec3f() };
	vertices[45] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	
	vertices[46] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f + (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };
	vertices[47] = Vertex{ vec3f(0.0f + (size.X / 2.0f), 0.0f - (size.Y / 2.0f), 0.0f + (size.Z / 2.0f)), vec3f() };

	Mesh res;

	if (reload)
	{
		if (mesh)
		{
			Vertex *verts = mesh->GetBuffers().OpenVertexBuffer();

			for (uint32 i = 0; i < 48; i++)
			{
				verts[i].Position = vertices[i].Position;
			}

			CalculateNormals(vertices, 48);

			mesh->CalculateBounds();
			mesh->CalculateSize();
		}
	}
	else
	{
		res = CreateMesh(vertices, 48, false, false);

		res.GetBuffers().SetPrimitiveType(GL_LINES);
	}

	return res;
}

Mesh MeshFactory::CreateGrid(float horizontal, float vertical, float spacing, char *plane, bool reload, Mesh *mesh)
{
	std::vector<Vertex> vertices;
	vec3f startPos;

	if (plane == "xz" || plane == "zx" || plane == "XZ" || plane == "ZX")
	{
		startPos = vec3f(0.0f - (horizontal / 2.0f), 0.0f, 0.0f + (vertical / 2.0f));

		while (startPos.X <= (horizontal / 2.0f))
		{
			vec3f endPos(startPos.X, startPos.Y, startPos.Z);
			endPos.Z -= vertical;

			vertices.push_back(Vertex{ vec3f(startPos.X, startPos.Y, startPos.Z), vec3f() });
			vertices.push_back(Vertex{ vec3f(endPos.X, endPos.Y, endPos.Z), vec3f() });

			startPos.X += spacing;
		}

		startPos = vec3f(0.0f - (horizontal / 2.0f), 0.0f, 0.0f + (vertical / 2.0f));

		while (startPos.Z >= -(vertical / 2.0f))
		{
			vec3f endPos(startPos.X, startPos.Y, startPos.Z);
			endPos.X += horizontal;

			vertices.push_back(Vertex{ vec3f(startPos.X, startPos.Y, startPos.Z), vec3f() });
			vertices.push_back(Vertex{ vec3f(endPos.X, endPos.Y, endPos.Z), vec3f() });

			startPos.Z -= spacing;
		}
	}
	else if (plane == "xy" || plane == "yx" || plane == "XY" || plane == "YX")
	{
		startPos = vec3f(0.0f - (horizontal / 2.0f), 0.0f - (vertical / 2.0f), 0.0f);

		while (startPos.X <= (horizontal / 2.0f))
		{
			vec3f endPos(startPos.X, startPos.Y, startPos.Z);
			endPos.Y += vertical;

			vertices.push_back(Vertex{ vec3f(startPos.X, startPos.Y, startPos.Z), vec3f() });
			vertices.push_back(Vertex{ vec3f(endPos.X, endPos.Y, endPos.Z), vec3f() });

			startPos.X += spacing;
		}

		startPos = vec3f(0.0f - (horizontal / 2.0f), 0.0f - (vertical / 2.0f), 0.0f);

		while (startPos.Y <= (vertical / 2.0f))
		{
			vec3f endPos(startPos.X, startPos.Y, startPos.Z);
			endPos.X += horizontal;

			vertices.push_back(Vertex{ vec3f(startPos.X, startPos.Y, startPos.Z), vec3f() });
			vertices.push_back(Vertex{ vec3f(endPos.X, endPos.Y, endPos.Z), vec3f() });

			startPos.Y += spacing;
		}
	}
	else if (plane == "yz" || plane == "zy" || plane == "YZ" || plane == "ZY")
	{
		startPos = vec3f(0.0f, 0.0f - (vertical / 2.0f), 0.0f + (horizontal / 2.0f));

		while (startPos.Z >= -(horizontal / 2.0f))
		{
			vec3f endPos(startPos.X, startPos.Y, startPos.Z);
			endPos.Y += vertical;

			vertices.push_back(Vertex{ vec3f(startPos.X, startPos.Y, startPos.Z), vec3f() });
			vertices.push_back(Vertex{ vec3f(endPos.X, endPos.Y, endPos.Z), vec3f() });

			startPos.Z -= spacing;
		}

		startPos = vec3f(0.0f, 0.0f - (vertical / 2.0f), 0.0f + (horizontal / 2.0f));

		while (startPos.Y <= (vertical / 2.0f))
		{
			vec3f endPos(startPos.X, startPos.Y, startPos.Z);
			endPos.Z -= horizontal;

			vertices.push_back(Vertex{ vec3f(startPos.X, startPos.Y, startPos.Z), vec3f() });
			vertices.push_back(Vertex{ vec3f(endPos.X, endPos.Y, endPos.Z), vec3f() });

			startPos.Y += spacing;
		}
	}
	else
	{
		startPos = vec3f(0.0f - (horizontal / 2.0f), 0.0f - (vertical / 2.0f), 0.0f);

		while (startPos.X <= (horizontal / 2.0f))
		{
			vec3f endPos(startPos.X, startPos.Y, startPos.Z);
			endPos.Y += vertical;

			vertices.push_back(Vertex{ vec3f(startPos.X, startPos.Y, startPos.Z), vec3f() });
			vertices.push_back(Vertex{ vec3f(endPos.X, endPos.Y, endPos.Z), vec3f() });

			startPos.X += spacing;
		}

		startPos = vec3f(0.0f - (horizontal / 2.0f), 0.0f - (vertical / 2.0f), 0.0f);

		while (startPos.Y <= (vertical / 2.0f))
		{
			vec3f endPos(startPos.X, startPos.Y, startPos.Z);
			endPos.X += horizontal;

			vertices.push_back(Vertex{ vec3f(startPos.X, startPos.Y, startPos.Z), vec3f() });
			vertices.push_back(Vertex{ vec3f(endPos.X, endPos.Y, endPos.Z), vec3f() });

			startPos.Y += spacing;
		}
	}

	Mesh res;

	if (reload)
	{
		if (mesh)
		{
			Vertex *verts = mesh->GetBuffers().OpenVertexBuffer();

			for (uint32 i = 0; i < vertices.size(); i++)
			{
				verts[i].Position = vertices[i].Position;
			}

			CalculateNormals(&vertices[0], vertices.size());

			mesh->CalculateBounds();
			mesh->CalculateSize();
		}
	}
	else
	{
		res = CreateMesh(&vertices[0], vertices.size(), false, false);

		res.GetBuffers().SetPrimitiveType(GL_LINES);
	}

	return res;
}

Mesh MeshFactory::CreateHexagonGrid(vec3f bottomLeft,float r, float nh, float nv, float lw, char *direction)
{
	vec3f n, b, a;
	float edges = 6;

	if (direction == "xy" || direction == "yx" || direction == "XY" || direction == "YX")
	{
		n = vec3f(0, 0, 1);
		b = vec3f(1, 0, 0);
		a = vec3f(0, 1, 0);
	}
	else if (direction == "xz" || direction == "zx" || direction == "XZ" || direction == "ZX")
	{
		n = vec3f(0, 1, 0);
		b = vec3f(1, 0, 0);
		a = vec3f(0, 0, 1);
	}
	else if (direction == "yz" || direction == "zy" || direction == "YZ" || direction == "ZY")
	{
		n = vec3f(1, 0, 0);
		b = vec3f(0, 0, 1);
		a = vec3f(0, 1, 0);
	}
	else
	{
		n = vec3f(0, 1, 0);
		b = vec3f(1, 0, 0);
		a = vec3f(0, 0, 1);
	}

	std::vector<Vertex> vertices;
	std::vector<Vertex> allVertices;
	
	for (uint32 i = 0; i < edges; i++)
	{
		Vertex q[2];

		float t1 = float(i) * 2.0f * PI / float(edges);
		float t2 = float(i + 1) * 2.0f * PI / float(edges);

		q[0].Position = bottomLeft + r * cosf(t1 + ToRadians(0.0f)) * a + r * sinf(t1 + ToRadians(0.0f)) * b;
		q[1].Position = bottomLeft + r * cosf(t2 + ToRadians(0.0f)) * a + r * sinf(t2 + ToRadians(0.0f)) * b;
		
		vertices.push_back(q[1]);
		vertices.push_back(q[0]);

		vertices.push_back(q[0]);
		vertices.push_back(q[1]);

		//vertices.push_back(q[0]);
		//vertices.push_back(center);

		/*allVertices.push_back(q[1]);
		allVertices.push_back(q[0]);*/
		//allVertices.push_back(center);
	}

	float offset1 = 0;
	float offset2 = 0;
	
	for (uint32 k = 0; k < nv; k++)
	{
		for (uint32 i = 0; i < nh; i++)
		{
			offset1 += (r * sinf(2.0f * PI / float(edges)) * 2.0f);

			for (uint32 j = 0; j < vertices.size(); j++)
			{
				Vertex v = vertices[j];

				v.Position += offset1 * b;
				v.Position += offset2 * a;

				allVertices.push_back(v);
			}
		}

		offset2 += (r * 30.0f) / 20.0f;
		//offset2 += (r * cosf(2.0f * PI / float(edges))) + r;
		
		/*a *= offset2;
	
		a = Vec3::Normalize(a);*/
		if (k % 2 == 0)
		{
			offset1 = ((r * 17.0f) / 20.0f);
		}
		else
		{
			offset1 = 0;
		}
	}

	Mesh res;

	res = CreateMesh(&allVertices[0], allVertices.size(), 0, 0, vec2f());

	res.GetBuffers().SetPrimitiveType(GL_LINES);
	res.GetBuffers().SetLineWidth(lw);

	return res;
}



void MeshFactory::Triangle(std::vector<Vertex> &vertices, vec3f &a, vec3f &b, vec3f &c) 
{
	vertices.push_back({ a, vec3f(), vec2f() });
	vertices.push_back({ b, vec3f(), vec2f() });
	vertices.push_back({ c, vec3f(), vec2f() });
}

void MeshFactory::DivideTriangle(std::vector<Vertex> &vertices, vec3f &a, vec3f &b, vec3f &c, int iter, float rad)
{
	if (iter > 0)
	{
		vec3f v1 = Vec3::Normalize((a + b) / 2.0f);// * rad;
		vec3f v2 = Vec3::Normalize((b + c) / 2.0f);// * rad;
		vec3f v3 = Vec3::Normalize((c + a) / 2.0f);// * rad;

		DivideTriangle(vertices, a, v1, v3, iter - 1, rad);
		DivideTriangle(vertices, v1, b, v2, iter - 1, rad);
		DivideTriangle(vertices, v1, v2, v3, iter - 1, rad);
		DivideTriangle(vertices, v3, v2, c, iter - 1, rad);
	}
	else
	{
		Triangle(vertices, a * rad, b * rad, c * rad);
	}
}

Mesh MeshFactory::CreateTetrahedronSphere(float rad, int recursionLevel, bool withTextures, bool reload, Mesh * mesh, vec2f & ts)
{
	std::vector<Vertex> vertices;

	vec3f prism[4] = 
	{
		vec3f(0.0f, 0.0f, 1.0f),
		vec3f(0.0f, 0.94f, -0.33f),
		vec3f(-0.81f, -0.47f, -0.33f),
		vec3f(0.81f, -0.47f, -0.33f),
	};

	DivideTriangle(vertices, prism[0], prism[1], prism[2], recursionLevel, rad);
	DivideTriangle(vertices, prism[0], prism[3], prism[1], recursionLevel, rad);
	DivideTriangle(vertices, prism[0], prism[2], prism[3], recursionLevel, rad);
	DivideTriangle(vertices, prism[3], prism[2], prism[1], recursionLevel, rad);

	Mesh res;

	if (reload)
	{
		if (mesh)
		{
			Vertex *verts = mesh->GetBuffers().OpenVertexBuffer();

			for (uint32 i = 0; i < vertices.size(); i++)
			{
				verts[i].Position = vertices[i].Position;
			}

			CalculateNormals(&vertices[0], vertices.size());

			mesh->CalculateBounds();
			mesh->CalculateSize();

			if (withTextures)
			{
				CalculateTextureCoordinates(&vertices[0], vertices.size(), mesh->GetMinBounds(), mesh->GetMaxBounds(), ts);
			}
		}
	}
	else
	{
		res = CreateMesh(&vertices[0], vertices.size(), true, withTextures, ts);

		res.GetBuffers().SetPrimitiveType(GL_TRIANGLES);
	}

	return res;
}

Mesh MeshFactory::CreateSphere(vec3f rad, int recursionLevel, bool withTextures, bool reload, Mesh *mesh, vec2f &ts)
{
	IcoSphereCreator sc(rad);

	std::vector<Vertex> vertices = sc.Create(recursionLevel);

	Mesh res;

	if (reload)
	{
		if (mesh)
		{
			Vertex *verts = mesh->GetBuffers().OpenVertexBuffer();

			for (uint32 i = 0; i < vertices.size(); i++)
			{
				verts[i].Position = vertices[i].Position;
			}

			CalculateNormals(&vertices[0], vertices.size());

			mesh->CalculateBounds();
			mesh->CalculateSize();

			if (withTextures)
			{
				CalculateTextureCoordinates(&vertices[0], vertices.size(), mesh->GetMinBounds(), mesh->GetMaxBounds(), ts);
			}
		}
	}
	else
	{
		res = CreateMesh(&vertices[0], vertices.size(), true, withTextures, ts);

		res.GetBuffers().SetPrimitiveType(GL_TRIANGLES);
	}

	return res;
}

Mesh MeshFactory::CreateCylinder(float h, vec3f r1, vec3f r2, int m, bool withTextures, bool reload, Mesh *mesh, vec2f &ts)
{
	vec3f p1 = vec3f(0, -h / 2.0f, 0);
	vec3f p2 = vec3f(0, h / 2.0f, 0);

	int i, j;
	float t;
	vec3f n, p, a, b;

	n = p2 - p1;

	b = vec3f(1, 0, 0);
	a = Vec3::Normalize(Vec3::Cross(n, b));
	b = Vec3::Normalize(Vec3::Cross(a, n));
	
	std::vector<Vertex> vertices;
	for (uint32 i = 0; i < m; i++)
	{
		Vertex q[4];

		float t1 = float(i) * 2.0f * PI / float(m);
		float t2 = float(i + 1) * 2.0f * PI / float(m);

		q[0].Position = p1 + r1 * cosf(t1) * a + r1 * sinf(t1) * b;
		q[1].Position = p2 + r2 * cosf(t1) * a + r2 * sinf(t1) * b;
		q[2].Position = p2 + r2 * cosf(t2) * a + r2 * sinf(t2) * b;
		q[3].Position = p1 + r1 * cosf(t2) * a + r1 * sinf(t2) * b;

		vertices.push_back(q[0]);
		vertices.push_back(q[1]);
		vertices.push_back(q[2]);

		vertices.push_back(q[0]);
		vertices.push_back(q[2]);
		vertices.push_back(q[3]);

		Vertex c[3];
		
		if (r2 != vec3f())
		{
			c[0].Position = p2;
			c[1].Position = q[1].Position;
			c[2].Position = q[2].Position;

			vertices.push_back(c[2]);
			vertices.push_back(c[1]);
			vertices.push_back(c[0]);
		}

		if (r1 != vec3f())
		{
			c[0].Position = p1;
			c[1].Position = q[0].Position;
			c[2].Position = q[3].Position;

			vertices.push_back(c[0]);
			vertices.push_back(c[1]);
			vertices.push_back(c[2]);
		}			
	}

	Mesh res;

	if (reload)
	{
		if (mesh)
		{
			Vertex *verts = mesh->GetBuffers().OpenVertexBuffer();

			for (uint32 i = 0; i < vertices.size(); i++)
			{
				verts[i].Position = vertices[i].Position;
			}

			CalculateNormals(&vertices[0], vertices.size());

			mesh->CalculateBounds();
			mesh->CalculateSize();

			if (withTextures)
			{
				CalculateTextureCoordinates(&vertices[0], vertices.size(), mesh->GetMinBounds(), mesh->GetMaxBounds(), ts);
			}
		}
	}
	else
	{
		res = CreateMesh(&vertices[0], vertices.size(), true, withTextures, ts);

		res.GetBuffers().SetPrimitiveType(GL_TRIANGLES);
	}

	return res;
}	

int IcoSphereCreator::AddVertex(Vertex v)
{
	Vertex temp = v;
	temp.Position = Vec3::Normalize(v.Position);
	temp.Position *= Raduis;
	_Vertices.push_back(temp);
	return Index++;
}

int IcoSphereCreator::GetMiddlePoint(int p1, int p2)
{
	// first check if we have it already
	bool firstIsSmaller = p1 < p2;
	uint64 smallerIndex = firstIsSmaller ? p1 : p2;
	uint64 greaterIndex = firstIsSmaller ? p2 : p1;
	uint64 key = (smallerIndex << 32) + greaterIndex;

	int ret;
	if (MiddlePointIndexCache.find(key) != MiddlePointIndexCache.end())
	{
		ret = MiddlePointIndexCache[key];
		return ret;
	}

	// not in cache, calculate it
	vec3f point1 = _Vertices[p1].Position;
	vec3f point2 = _Vertices[p2].Position;
	vec3f middle = vec3f(
		(point1.X + point2.X) / 2.0f,
		(point1.Y + point2.Y) / 2.0f,
		(point1.Z + point2.Z) / 2.0f);

	// add vertex makes sure point is on unit sphere
	int i = AddVertex(Vertex{ middle, vec3f() });

	// store it, return index
	MiddlePointIndexCache[key] = i;

	return i;
}

std::vector<Vertex>& IcoSphereCreator::Create(int recursionLevel)
{
	Index = 0;

	// create 12 vertices of a icosahedron
	float t = (1.0f + std::sqrtf(5.0f)) / 2.0f;

	AddVertex(Vertex{ vec3f(-1, t, 0) });
	AddVertex(Vertex{ vec3f(1, t, 0) });
	AddVertex(Vertex{ vec3f(-1, -t, 0) });
	AddVertex(Vertex{ vec3f(1, -t, 0) });

	AddVertex(Vertex{ vec3f(0, -1, t) });
	AddVertex(Vertex{ vec3f(0, 1, t) });
	AddVertex(Vertex{ vec3f(0, -1, -t) });
	AddVertex(Vertex{ vec3f(0, 1, -t) });

	AddVertex(Vertex{ vec3f(t, 0, -1) });
	AddVertex(Vertex{ vec3f(t, 0, 1) });
	AddVertex(Vertex{ vec3f(-t, 0, -1) });
	AddVertex(Vertex{ vec3f(-t, 0, 1) });


	// create 20 triangles of the icosahedron
	std::vector<TriangleIndices> faces;

	// 5 faces around point 0
	faces.push_back(TriangleIndices{ 0, 11, 5 });
	faces.push_back(TriangleIndices{ 0, 5, 1 });
	faces.push_back(TriangleIndices{ 0, 1, 7 });
	faces.push_back(TriangleIndices{ 0, 7, 10 });
	faces.push_back(TriangleIndices{ 0, 10, 11 });

	// 5 adjacent faces 
	faces.push_back(TriangleIndices{ 1, 5, 9 });
	faces.push_back(TriangleIndices{ 5, 11, 4 });
	faces.push_back(TriangleIndices{ 11, 10, 2 });
	faces.push_back(TriangleIndices{ 10, 7, 6 });
	faces.push_back(TriangleIndices{ 7, 1, 8 });

	// 5 faces around point 3
	faces.push_back(TriangleIndices{ 3, 9, 4 });
	faces.push_back(TriangleIndices{ 3, 4, 2 });
	faces.push_back(TriangleIndices{ 3, 2, 6 });
	faces.push_back(TriangleIndices{ 3, 6, 8 });
	faces.push_back(TriangleIndices{ 3, 8, 9 });

	// 5 adjacent faces 
	faces.push_back(TriangleIndices{ 4, 9, 5 });
	faces.push_back(TriangleIndices{ 2, 4, 11 });
	faces.push_back(TriangleIndices{ 6, 2, 10 });
	faces.push_back(TriangleIndices{ 8, 6, 7 });
	faces.push_back(TriangleIndices{ 9, 8, 1 });

	// refine triangles
	for (int i = 0; i < recursionLevel; i++)
	{
		std::vector<TriangleIndices> faces2;

		for each(auto tri in faces)
		{
			// replace triangle by 4 triangles
			int a = GetMiddlePoint(tri.V1, tri.V2);
			int b = GetMiddlePoint(tri.V2, tri.V3);
			int c = GetMiddlePoint(tri.V3, tri.V1);

			faces2.push_back(TriangleIndices{ tri.V1, a, c });
			faces2.push_back(TriangleIndices{ tri.V2, b, a });
			faces2.push_back(TriangleIndices{ tri.V3, c, b });
			faces2.push_back(TriangleIndices{ a, b, c });
		}

		faces = faces2;
	}

	// done, now add triangles to mesh
	for each(auto tri in faces)
	{
		Vertices.push_back(_Vertices[tri.V1]);
		Vertices.push_back(_Vertices[tri.V2]);
		Vertices.push_back(_Vertices[tri.V3]);
	}

	return Vertices;
}