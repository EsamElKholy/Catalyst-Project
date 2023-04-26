#include <MeshRenderer.h>

void MeshRenderer::Init() 
{
	
}

void MeshRenderer::Update() 
{
	//Owner->GetTransform()->Rotate(1, vec3f(1, 0, 0));
}

void MeshRenderer::Render()
{
	if (MainShader)
	{
		if (UseFog)
		{
			SecShader->Activate();

			//Skin.BindTexture();
			mat4f m = Owner->GetTransform()->GetModelMatrix();

			((FogShader*)SecShader)->UpdateUniforms(m, Owner->GetScene()->GetMainCamera(), Color, FogColor, Start, End, FogP);

			Renderable.GetBuffers().DrawVertices();			
		}
		else if (UseDirectional)
		{
			MainShader->Activate();

			Skin.BindTexture();
			mat4f m = Owner->GetTransform()->GetModelMatrix();

			((DirectionalShader*)MainShader)->UpdateUniforms(m, *Owner->GetScene()->GetMainCamera(), Color);

			Renderable.GetBuffers().DrawVertices();
			
			Skin.UnbindTexture();
		}
		else
		{
			MainShader->Activate();

			//Skin.BindTexture();
			mat4f m = Owner->GetTransform()->GetModelMatrix();

			((BasicShader*)MainShader)->UpdateUniforms(m, Owner->GetScene()->GetMainCamera(), Color);

			Renderable.GetBuffers().DrawVertices();
		}
		
	}
}

void MeshRenderer::Destroy()
{
	Renderable.Destroy();
	MainShader->Unload();
	//Skin.UnloadTexture();
}