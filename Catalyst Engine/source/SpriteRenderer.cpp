#include <SpriteRenderer.h>
#include <Actor.h>
#include <IComponent.h>
#include "SceneManager.h"
#include "Transform.h"

void SpriteRenderer::Init()
{
	
}

void SpriteRenderer::Update()
{
	//Owner->GetTransform()->Rotate(1, vec3f(1, 0, 0));
}

void SpriteRenderer::Render()
{
	if (MainShader)
	{				
		MainShader->Activate();

		Skin.BindTexture();
		mat4f m = Owner->GetTransform()->GetModelMatrix();

		((DirectionalShader*)MainShader)->UpdateUniforms(m, *Owner->GetScene()->GetMainCamera(), Color);

		Sprite.GetBuffers().DrawVertices();

		Skin.UnbindTexture();
	}
}

void SpriteRenderer::Destroy()
{
	Sprite.Destroy();
	MainShader->Unload();
	//Skin.UnloadTexture();
}