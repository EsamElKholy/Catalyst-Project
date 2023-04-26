#pragma once

#include <Catalyst.h>
#include <InputManager.h>

class LineSketcher : public IComponent
{
public:
	LineSketcher(const std::string &name, const std::string &type, vec3f color, Shader *shader)
		: IComponent(name, type), Color(color), MainShader(shader)
	{}
	~LineSketcher() {}

public:
	void Init() override 
	{
		Drawing = false;
		CurrentLine = 0;
	}

	void Update() override 
	{
		vec2f mp = InputManager::GetMousePos();

		if (InputManager::IsKeyUp(GLFW_KEY_Z))
		{
			if (Points.size() > 0)
			{
				Points[Points.size() - 1].clear();
				Points.pop_back();
			}

			if (Lines.size() > 0)
			{
				Lines[Lines.size() - 1].Destroy();
				Lines.pop_back();
			}

			CurrentLine = Points.size();
		}

		if (InputManager::IsKeyUp(GLFW_KEY_R))
		{
			while (Points.size() > 0)
			{
				Points[Points.size() - 1].clear();
				Points.pop_back();
			}

			while (Lines.size() > 0)
			{
				Lines[Lines.size() - 1].Destroy();
				Lines.pop_back();
			}

			CurrentLine = 0;
		}

		if (InputManager::IsButtonDown(0) && !Drawing)
		{
			Drawing = true;
			Vertex v;
			v.Position = vec3f(mp.X, mp.Y);
			std::vector<Vertex> verts;
			Points.push_back(verts);
			Points[CurrentLine].push_back(v);
			Mesh mesh;
			Lines.push_back(mesh);
		}

		if (InputManager::IsButtonUp(0) && Drawing)
		{
			Drawing = false;
			/*Mesh mesh = MeshFactory::CreateMesh(&Points[CurrentLine][0], Points[CurrentLine].size(), false, false);
			mesh.GetBuffers().SetPrimitiveType(GL_LINE_STRIP);

			Lines.push_back(mesh);*/
			CurrentLine++;
		}

		if (Drawing && OldMousePos.X != mp.X && OldMousePos.Y != mp.Y)
		{
			Vertex v;
			v.Position = vec3f(mp.X, mp.Y);
			Points[CurrentLine].push_back(v);

			Lines[CurrentLine].Destroy();
			Lines[CurrentLine] = MeshFactory::CreateMesh(&Points[CurrentLine][0], Points[CurrentLine].size(), false, false, vec2f());
			Lines[CurrentLine].GetBuffers().SetPrimitiveType(GL_LINE_STRIP);

			OldMousePos = mp;
		}
	}

	void Render() override 
	{
		MainShader->Activate();

		mat4f m = Owner->GetTransform()->GetModelMatrix();

		for (int i = 0; i < Lines.size(); i++)
		{
			((BasicShader*)MainShader)->UpdateUniforms(m, Owner->GetScene()->GetMainCamera(), Color);
			Lines[i].GetBuffers().DrawVertices();
		}
	}

	void Destroy() override {}
private:
	std::vector<std::vector<Vertex>> Points;
	std::vector<Mesh> Lines;
	vec2f OldMousePos;
	Shader *MainShader;
	vec3f Color; 
	int CurrentLine;
	bool Drawing;
};
