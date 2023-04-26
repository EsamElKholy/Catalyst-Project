#pragma once

#include "Texture.h"
#include "Lightmap Shader.h"
#include "Mesh.h"
#include "IComponent.h"
#include "Actor.h"
#include "SceneManager.h"
#include "Transform.h"

class LightsRenderer : public IComponent
{
public:
	LightsRenderer(const std::string &name, const std::string &type, Mesh &mesh, LightmapShader *shader) 
		: IComponent(name, type), Renderable(mesh), MainShader(shader)
	{		
	}

	LightsRenderer(const std::string &name, const std::string &type, Mesh &mesh, LightmapShader *shader, Material mat)
		: IComponent(name, type), Renderable(mesh), MainShader(shader), Mat(mat), Color(vec3f(1, 1, 1))
	{
	}

	LightsRenderer(const std::string &name, const std::string &type, Mesh &mesh, vec3f col, LightmapShader *shader, Material mat)
		: IComponent(name, type), Renderable(mesh), MainShader(shader), Mat(mat), Color(col)
	{
	}

	~LightsRenderer() 
	{}

public:
	virtual void Init() override {}

	virtual void Update() override 
	{
		if (PointLights.size() > 0)
		{
//			PointLights[0].Position = Owner->GetScene()->GetMainCamera()->GetPosition();
		}
	}

	virtual void Render() override 
	{
		if (MainShader)
		{
			mat4f m = Owner->GetTransform()->GetModelMatrix();

			bool withDiffueTexture = Mat.DiffuseSkin.IsAvailable();
			bool withSpecularTexture = Mat.SpecularSkin.IsAvailable();

			MainShader->UpdateUniforms(m, *(Owner->GetScene()->GetMainCamera()), Color, Mat.Shininess, GetPointLights(), GetLightCount(), DirLight);

			if (withDiffueTexture)
			{
				Mat.DiffuseSkin.BindTexture(0);
			}

			if (withSpecularTexture)
			{
				Mat.SpecularSkin.BindTexture(1);
			}

			Renderable.GetBuffers().DrawVertices();

			if (withDiffueTexture)
			{
				Mat.DiffuseSkin.UnbindTexture();
			}

			if (withSpecularTexture)
			{
				Mat.SpecularSkin.UnbindTexture();
			}
		}
	}

	virtual void Destroy() override {}

public:
	void SetMaterial(Material mat) { Mat = mat; }
	void SetShader(LightmapShader *shader) { MainShader = shader; }
	void SetRenderable(Mesh &mesh) { Renderable = mesh; }
	void SetDirectionalLight(DirectionalLight &dir) { DirLight = dir; }
	void AddPointLight(PointLight &light) 
	{
		if (PointLights.size() < 10)
		{
			PointLights.push_back(light);
		}
	}

	uint32 GetLightCount() { return PointLights.size(); }
	PointLight* GetPointLights() 
	{
		if (PointLights.size() > 0)
		{
			return &PointLights[0];
		}

		return NULL;
	}

	Mesh GetRenderable() { return Renderable; }

private:
	LightmapShader *MainShader;
	std::vector<PointLight> PointLights;
	DirectionalLight DirLight;
	Mesh Renderable;
	Material Mat;
	vec3f Color;
};
