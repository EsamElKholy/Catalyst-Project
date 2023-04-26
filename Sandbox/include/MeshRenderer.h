#pragma once

#include <Catalyst.h>

class MeshRenderer : public IComponent
{
public:
	MeshRenderer(const std::string &name, const std::string &type, Mesh &mesh, vec3f color, Shader *shader, bool useDir = 0)
		: IComponent(name, type), Color(color), MainShader(shader), UseDirectional(useDir)
	{
		Renderable = mesh;
		//Skin = Texture("Resources/Textures/container2.png");
	}

	MeshRenderer(const std::string &name, const std::string &type, Mesh &mesh, vec3f color, Texture tex, Shader *shader)
		: IComponent(name, type), Color(color), MainShader(shader)
	{
		Renderable = mesh;
		Skin = tex;
		UseDirectional = true;
	}
	~MeshRenderer() {}

public:
	virtual void Init() override;

	virtual void Update() override;
	virtual void Render() override;

	virtual void Destroy() override;

public:
	Mesh& GetRenderable() { return Renderable; }
	vec3f GetColor() { return Color; }
	void SetFog(FogShader *shader, vec3f col, float start, float end, float p) 
	{
		SecShader = shader;
		FogColor = col;
		Start = start;
		End = end;
		FogP = p;
		UseFog = true;
	}

private:
	Shader *MainShader;
	FogShader *SecShader;
	Texture Skin;
	Mesh Renderable;
	vec3f Color;
	vec3f FogColor;
	float Start;
	float End;
	float FogP;
	bool UseFog;
	bool UseDirectional;
};