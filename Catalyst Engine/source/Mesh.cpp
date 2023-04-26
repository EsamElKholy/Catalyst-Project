#include <Mesh.h>
	
void Mesh::CalculateBounds()
{
	vec3f min(999999999.0f, 999999999.0f, 999999999.0f);
	vec3f max(-999999999.0f, -999999999.0f, -999999999.0f);

	Vertex *vertices = MeshBuffers.OpenVertexBuffer();

	for (uint32 i = 0; i < MeshBuffers.GetVertexCount(); i++)
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

	MinBounds = min;
	MaxBounds = max;

	MeshBuffers.CloseVertexBuffer();
}

void Mesh::CalculateSize()
{
	Size = MaxBounds - MinBounds;
}

void Mesh::CalculateVolume()
{
	Vertex *vertices = MeshBuffers.OpenVertexBuffer();

	float vol = 0;

	for (uint32 i = 0; i < MeshBuffers.GetVertexCount(); i += 3)
	{
		vec3f p1 = vertices[i + 0].Position;
		vec3f p2 = vertices[i + 1].Position;
		vec3f p3 = vertices[i + 2].Position;

		vol += CalculateSignedVolume(p1, p2, p3);
	}

	Volume = fabsf(vol);

	MeshBuffers.CloseVertexBuffer();
}

float Mesh::CalculateSignedVolume(vec3f & p1, vec3f & p2, vec3f & p3)
{
	return Vec3::Dot(p1, Vec3::Cross(p2, p3)) / 6.0f;
}

void Mesh::Destroy()
{
	MeshBuffers.Destroy();
}

void Buffers::BindBuffers()
{
	if (VAO && VBO)
	{
		glBindVertexArray(VAO);
	}
}

void Buffers::UnbindBuffer()
{
	if (VAO && VBO)
	{
		glBindVertexArray(0);
	}
}

Vertex* Buffers::OpenVertexBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	Vertex *vertices = (Vertex*)glMapBufferRange(GL_ARRAY_BUFFER, 0, VertexCount * sizeof(Vertex), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return vertices;
}

void Buffers::CloseVertexBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Buffers::DrawVertices()
{
	BindBuffers();

	if (PrimitiveType == GL_LINES)
	{
		glLineWidth(LineWidth);
	}

	glDrawArrays(PrimitiveType, 0, VertexCount);

	if (PrimitiveType == GL_LINES)
	{
		glLineWidth(1.0f);
	}

	UnbindBuffer();
}

void Buffers::Destroy()
{
	if (VBO)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}
	else
	{
		VBO = 0;
	}

	if (VAO)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
	else
	{
		VAO = 0;
	}
}

