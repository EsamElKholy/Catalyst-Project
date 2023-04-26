#pragma once

#include "Shader.h"

class DirectionalShader : public Shader
{
public:
	DirectionalShader(char *vertPath = "Resources/Shaders/directional shader.vert"
		, char *fragPath = "Resources/Shaders/directional shader.frag")
		: Shader(vertPath, fragPath)
	{
		CreateShaders();
		Activate();

		AddAttribute("position", 0);
		AddAttribute("normal", 1);
		AddAttribute("texcoords", 1);

		AddUniform("model");
		AddUniform("view");
		AddUniform("projection");

		AddUniform("eyePos");

		AddUniform("color");

		AddUniform("dirLight.direction");
		AddUniform("dirLight.ambient");
		AddUniform("dirLight.ambientStrength");
		AddUniform("dirLight.diffuse");
		AddUniform("dirLight.specular");

		AddUniform("myTex");

		Deactivate();
	}

	~DirectionalShader()
	{
		//Unload();
	}

public:
	void UpdateUniforms(mat4f &m, Camera &cam, vec3f &c);
};