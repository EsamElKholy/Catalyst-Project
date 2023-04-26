#pragma once

#include "Shader.h"

class BasicShader : public Shader
{
public:
	BasicShader(char *vertPath = "Resources/Shaders/basic shader.vert"
		, char *fragPath = "Resources/Shaders/basic shader.frag")
		: Shader(vertPath, fragPath)
	{
		CreateShaders();
		Activate();

		AddAttribute("position", 0);
		AddAttribute("normal", 1);

		AddUniform("model");
		AddUniform("view");
		AddUniform("projection");

		AddUniform("color");
	}

	~BasicShader()
	{
		//Unload();
	}

public:
	void UpdateUniforms(mat4f & m, Camera *cam, vec3f & c);
};
