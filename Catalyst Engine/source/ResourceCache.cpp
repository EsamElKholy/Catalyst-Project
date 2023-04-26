#include <ResourceCache.h>

void ResourceCache::AddTexture(char *path, char *name)
{
	if (TextureCache.find(name) == TextureCache.end())
	{
		Texture texture = Texture(path);

		TextureCache[name] = texture;
	}
}

Texture ResourceCache::GetTexture(char * name)
{
	if (TextureCache.find(name) != TextureCache.end())
	{
		return TextureCache[name];
	}

	Texture texture;

	return texture;
}

void ResourceCache::RemoveTexture(char *name)
{
	Texture texture = GetTexture(name);

	if (texture.TextureID)
	{
		texture.UnloadTexture();

		TextureCache.erase(name);
	}
}

void ResourceCache::AddMaterial(char *name, Material material)
{
	if (MaterialCache.find(name) == MaterialCache.end())
	{
		MaterialCache[name] = material;
	}
}

Material ResourceCache::GetMaterial(char *name)
{
	if (MaterialCache.find(name) != MaterialCache.end())
	{
		return MaterialCache[name];
	}

	return Material{};
}

void ResourceCache::RemoveMaterial(char *name)
{
	Material material = GetMaterial(name);

	if (MaterialCache.find(name) != MaterialCache.end())
	{
		material.DiffuseSkin.UnloadTexture();
		material.SpecularSkin.UnloadTexture();

		MaterialCache.erase(name);
	}
}
