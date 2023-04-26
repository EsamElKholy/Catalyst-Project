#pragma once

#include "Texture.h"
#include <map>

class ResourceCache
{
public:
	ResourceCache() {}
	~ResourceCache() {}

public:
	void AddTexture(char *path, char *name);
	Texture GetTexture(char *name);
	void RemoveTexture(char *name);

	void AddMaterial(char *name, Material material);
	Material GetMaterial(char *name);
	void RemoveMaterial(char *name);

private:
	std::map<char *, Texture> TextureCache;
	std::map<char *, Material> MaterialCache;
};