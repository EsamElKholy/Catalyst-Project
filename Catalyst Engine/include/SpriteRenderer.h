#pragma once

#include "IComponent.h"
#include "DirectionalShader.h"
#include "Mesh.h"
#include "MeshFactory.h"
#include "Texture.h"

class SpriteRenderer : public IComponent
{
public:
	SpriteRenderer(const std::string &name, const std::string &type, Texture skin, vec3f size, vec3f color, Shader *shader)
		: IComponent(name, type), Color(color), MainShader(shader), Skin(skin)
	{
		Sprite = MeshFactory::CreateSprite(size);
		//Skin = Texture("Resources/Textures/container2.png");
	}

	~SpriteRenderer() {}

public:
	virtual void Init() override;

	virtual void Update() override;
	virtual void Render() override;

	virtual void Destroy() override;

public:
	Texture& GetSkin() { return Skin; }
	void SetSkin(Texture &skin) { Skin = skin; }

	Mesh& GetSprite() { return Sprite; }
	vec3f GetColor() { return Color; }	

private:
	Shader *MainShader;
	Texture Skin;
	Mesh Sprite;
	vec3f Color;
	bool UseDirectional;
};