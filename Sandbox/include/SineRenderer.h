#pragma once

#include <Catalyst.h>

class SineRenderer : public IComponent
{
public:
	SineRenderer(const std::string &name, const std::string &type, vec3f color, Shader *shader)
		: IComponent(name, type), Color(color), MainShader(shader)
	{}
	~SineRenderer() {}

public:
	void Init() override 
	{
		std::vector<Vertex> verts;

		float spacing = 1;
		float width = 800;
		float pc = width / spacing;
		float start = -width / 2.0f;

		for (float i = 0; i <= pc; i++)
		{
			Vertex v;
			v.Position = vec3f(start + (spacing * i * 12.0f));

			verts.push_back(v);
		}

		_Mesh = MeshFactory::CreateMesh(&verts[0], verts.size(), 0, 0, vec2f());
		_Mesh.GetBuffers().SetPrimitiveType(GL_LINE_STRIP);

		t = 0;
	}
	void Update() override 
	{
		t += 10.0f / 60.0f;
	}

	void Render() override 
	{
		MainShader->Activate();

		mat4f m = Owner->GetTransform()->GetModelMatrix();

		((SineShader*)MainShader)->UpdateUniforms(m, Owner->GetScene()->GetMainCamera(), Color, t);

		_Mesh.GetBuffers().DrawVertices();
	}

	void Destroy() override {}

private:
	Mesh _Mesh;
	Shader *MainShader;
	vec3f Color;
	float t;
};
