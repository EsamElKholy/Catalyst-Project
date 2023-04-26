#pragma once

#include "Shader.h"
#include "Camera.h"
#include "Lights.h"

class IComponent;

class LightmapShader : public Shader
{
public:
	LightmapShader(char *vertPath = "Resources/Shaders/lightmap shader.vert"
		, char *fragPath = "Resources/Shaders/lightmap shader.frag")
		: Shader(vertPath, fragPath)
	{
		CreateShaders();
		Activate();

		AddAttribute("position", 0);
		AddAttribute("normal", 1);
		AddAttribute("texcoord", 2);

		AddUniform("model");
		AddUniform("view");
		AddUniform("projection");

		AddUniform("eyePos");

		AddUniform("objectColor");
		
		AddUniform("objMaterial.Diffuse");
		AddUniform("objMaterial.Specular");
		AddUniform("objMaterial.Shininess");

		AddUniform("directional.Direction");
		AddUniform("directional.Ambient");
		AddUniform("directional.AmbientStrength");
		AddUniform("directional.Diffuse");
		AddUniform("directional.Specular");

		AddUniform("MAX_POINT_LIGHT_TO_USE");

		for (uint32 i = 0; i < 10; i++)
		{
			std::string s = "points[" + std::to_string(i) + "].";
			
			AddUniform(s + "Position");
			
			AddUniform(s + "Ambient");
			AddUniform(s + "Diffuse");
			AddUniform(s + "Specular");
			
			AddUniform(s + "AmbientStrength");
			
			AddUniform(s + "Atten.Constant");
			AddUniform(s + "Atten.Linear");
			AddUniform(s + "Atten.Quadratic");
		}

		Deactivate();
	}

	~LightmapShader()
	{
		//Unload();
	}

public:
	void UpdateUniforms(mat4f &m, Camera &cam, vec3f &c, float shininess, PointLight *lights, uint32 lightCount, DirectionalLight &dirLight)
	{
		Activate();

		SetUniformMat4("model", m);
		SetUniformMat4("view", cam.GetViewMatrix());
		SetUniformMat4("projection", cam.GetProjectionMatrix());
		SetUniform3f("eyePos", cam.GetActiveView().Position);

		SetUniform3f("objectColor", c);
		SetUniform1i("objMaterial.Diffuse", 0);
		SetUniform1i("objMaterial.Specular", 1);
		SetUniform1f("objMaterial.Shininess", shininess);

		SetUniform3f("directional.Direction", dirLight.Direction);
		SetUniform3f("directional.Ambient", dirLight.AmbientColor);
		SetUniform1f("directional.AmbientStrength", dirLight.AmbientStrength);
		SetUniform3f("directional.Diffuse", dirLight.DiffuseColor);
		SetUniform3f("directional.Specular", dirLight.SpecularColor);

		SetUniform1f("MAX_POINT_LIGHT_TO_USE", lightCount);

		for (uint32 i = 0; i < lightCount; i++)
		{
			std::string s = "points[" + std::to_string(i) + "].";

			SetUniform3f(s + "Position", lights[i].Position);

			SetUniform3f(s + "Ambient", lights[i].AmbientColor);
			SetUniform3f(s + "Diffuse", lights[i].DiffuseColor);
			SetUniform3f(s + "Specular", lights[i].SpecularColor);

			SetUniform1f(s + "AmbientStrength", lights[i].AmbientStrength);

			SetUniform1f(s + "Atten.Constant", lights[i].Atten.Constant);
			SetUniform1f(s + "Atten.Linear", lights[i].Atten.Linear);
			SetUniform1f(s + "Atten.Quadratic", lights[i].Atten.Quadratic);
		}
	}
};