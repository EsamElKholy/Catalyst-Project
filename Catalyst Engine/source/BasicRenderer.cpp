#include <BasicRenderer.h>
#include <IComponent.h>
#include <SceneManager.h>
#include <string>

void BasicRenderer::Init()
{
}

void BasicRenderer::RenderAll()
{
	if (Owner)
	{
		std::vector<IComponent*> *renderables = Owner->GetAllComponents("RENDERABLE");

		if (renderables)
		{
			for (uint32 i = 0; i < renderables->size(); i++)
			{
				if ((*renderables)[i]->GetOwner()->IsEnabled())
				{
					(*renderables)[i]->Render();
				}
			}
		}			
	}
}

void BasicRenderer::RenderSingle(IComponent *comp)
{
	if (comp)
	{
		if (comp->GetType() == "RENDERABLE")
		{
			comp->Render();
		}
	}
}

void BasicRenderer::Destroy()
{
}
