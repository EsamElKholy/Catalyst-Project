#pragma once

#include <GL\glew.h>
#include <cinder\CinderMath.h>

class Texture
{
public:
	Texture();
	Texture(char *path);

public:
	void BindTexture(int index = 0);
	void UnbindTexture();
	void UnloadTexture();
	bool IsAvailable();

public:
	uint32 GetTextureID() { return TextureID; }

private:
	void CreateTexture(char *path);
public:
	uint32 TextureID;
};

struct Material
{
	Texture DiffuseSkin;
	Texture SpecularSkin;

	float Shininess;
};
