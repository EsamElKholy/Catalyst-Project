#pragma once

#include "Shader.h"

class Camera;

class FogShader : public Shader
{
public:
	FogShader(char *vertPath = "Resources/Shaders/fog shader.vert"
		, char *fragPath = "Resources/Shaders/fog shader.frag")
		: Shader(vertPath, fragPath)
	{
		CreateShaders();
		Activate();

		AddAttribute("position", 0);
		AddAttribute("normal", 1);

		AddUniform("model");
		AddUniform("view");
		AddUniform("projection");

		AddUniform("eyePos");

		AddUniform("color");

		AddUniform("fogColor");
		AddUniform("start");
		AddUniform("end");
		AddUniform("fogP");
	}

	~FogShader()
	{
		//Unload();
	}

public:
	void UpdateUniforms(mat4f & m, Camera *cam, vec3f & c, vec3f &fogCol, float start, float end, float fogP);
};
