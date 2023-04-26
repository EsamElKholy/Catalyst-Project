#pragma once

#include "Shader.h"

class Camera;

class SineShader : public Shader
{
public:
	SineShader(char *vertPath = "Resources/Shaders/sine shader.vert"
		, char *fragPath = "Resources/Shaders/sine shader.frag")
		: Shader(vertPath, fragPath)
	{
		CreateShaders();
		Activate();

		AddAttribute("position", 0);
		AddAttribute("normal", 1);

		AddUniform("model");
		AddUniform("view");
		AddUniform("projection");
		AddUniform("increament");

		AddUniform("color");
	}

	~SineShader()
	{
		//Unload();
	}

public:
	void UpdateUniforms(mat4f & m, Camera *cam, vec3f & c, float t);
};
