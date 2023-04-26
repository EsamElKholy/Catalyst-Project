#include <SceneManager.h>
#include <BasicRenderer.h>
#include <Actor.h>
#include <IComponent.h>

void SceneManager::Init(IRenderingEngine *rengine)
{
	Root = NULL;

	Root = AddActor("Main Root");
	
	REngine = rengine;

	REngine->Init();

	REngine->SetOwner(this);
}

IActor* SceneManager::CreateObject(const std::string &name)
{
	IActor *actor = GetActor(name);

	if (!actor)
	{
		actor = new Actor();

		if (Root)
		{
			Actors[name] = actor;

			actor->SetParent(Root);
			actor->SetScene(this);
			actor->GetTransform()->SetParent(Root->GetTransform());
			actor->SetEnable(true);

			Root->AddChild(actor);
		}
		else
		{
			Actors[name] = actor;
			actor->SetScene(this);
			actor->SetTransform(new Transform());
			actor->SetEnable(true);
		}
	}

	return actor;
}

IActor* SceneManager::AddActor(const std::string &name)
{
	if (!GetActor(name))
	{
		return CreateObject(name);
	}

	return GetActor(name);
}

IActor* SceneManager::AddActor(IActor *actor)
{
	if (!GetActor(actor->GetName()))
	{
		if (actor)
		{
			if (Root)
			{
				if (actor->GetName().size() > 0)
				{
					Actors[actor->GetName()] = actor;

					actor->SetParent(Root);
					actor->SetScene(this);
					actor->GetTransform()->SetParent(Root->GetTransform());

					Root->AddChild(actor);
				}					
			}
		}
	}

	return actor;
}

IActor* SceneManager::GetActor(const std::string &name)
{
	if (Root)
	{
		if (name.size() > 0)
		{
			std::unordered_map<std::string, IActor*>::iterator it;
			it = Actors.find(name);

			if (it != Actors.end())
			{
				return it->second;
			}
		}
	}

	return NULL;
}

void SceneManager::RemoveAllComponents(const std::string &name)
{
	Actor *actor = (Actor*)GetActor(name);

	if (actor)
	{
		std::map<std::string, IComponent*> *comps = &(actor->GetAllComponents());

		for each (std::pair<std::string, IComponent*> c in *comps)
		{
			RemoveComponent(c.second);
		}

		//actor->RemoveAllComponents();
	}
}

void SceneManager::RemoveActor(const std::string &name)
{
	if (name != "Main Root")
	{
		IActor *actor = (Actor*)GetActor(name);

		if (actor)
		{
			RemoveAllComponents(name);
			
			actor->Destroy();

			actor->GetParent()->RemoveChild(name);
			
			Actors.erase(name);
		}
	}
}

void SceneManager::Render()
{
	REngine->RenderAll();

	if (Root)
	{
		Root->RenderAll();
	}
}

void SceneManager::Update()
{
	REngine->Update();

	if (Root)
	{
		Root->UpdateAll();
	}
}

void SceneManager::Destroy()
{
	REngine->Destroy();

	delete REngine;

	Root->Destroy();

	Actors.clear();
	AllComponents.clear();

	delete MainCamera;
	MainCamera = NULL;
	delete Root;
	Root = NULL;
}

std::vector<IComponent *>* SceneManager::GetAllComponents(std::string type)
{
	std::unordered_map<std::string, std::vector<IComponent*>>::iterator it = AllComponents.find(type);
	if (it != AllComponents.end())
	{
		return &AllComponents[type];
	}

	return NULL;
}

void SceneManager::RemoveComponent(IComponent *comp)
{
	if (comp)
	{
		std::vector<IComponent *> *components = GetAllComponents(comp->GetType());

		if (components)
		{
			int32 index = -1;
			for (uint32 i = 0; i < components->size(); i++)
			{
				if ((*components)[i] == comp)
				{
					index = i;
					break;
				}
			}

			if (index != -1)
			{
				components->erase(components->begin() + index);
			}
		}
	}
}

void SceneManager::AddComponentList(IComponent *component)
{
	if (AllComponents.find(component->GetType()) == AllComponents.end())
	{
		AllComponents[component->GetType()] = std::vector<IComponent *>();
	}
}

void SceneManager::RemoveComponentList(const std::string &type)
{
	std::vector<IComponent*> *list = GetAllComponents(type);

	if (list)
	{
		for (uint32 i = 0; i < list->size(); i++)
		{
			(*list)[i]->Destroy();
			delete (*list)[i];
		}

		AllComponents.erase(type);
	}
}

IComponent* SceneManager::AddComponent(IComponent *component)
{
	if (!component)
	{
		return NULL;
	}

	std::vector<IComponent *> *components = GetAllComponents(component->GetType());

	if (components)
	{
		AllComponents[component->GetType()].push_back(component);
		return component;
	}
	else
	{
		AllComponents[component->GetType()] = std::vector<IComponent *>();

		components = GetAllComponents(component->GetType());

		components->push_back(component);

		return component;
	}

	return NULL;
}