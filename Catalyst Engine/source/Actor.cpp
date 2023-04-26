#include <Actor.h>
#include <IComponent.h>
#include <Transform.h>
#include <SceneManager.h>

void Actor::Init()
{
	Enabled = true;
}

void Actor::Update()
{
	UpdateAll();
}

void Actor::Render()
{
	RenderAll();
}

void Actor::UpdateAll()
{
	if (ActorTransform)
	{
		ActorTransform->Update();
	}

	for each (std::pair<std::string, IComponent*> c in Components)
	{
		if (c.second->IsEnabled())
		{
			c.second->Update();
		}
	}

	for each (std::pair<std::string, IActor*> a in Children)
	{
		if (a.second->IsEnabled())
		{
			a.second->Update();
		}			
	}
}

void Actor::RenderAll()
{
	for each (std::pair<std::string, IComponent*> c in Components)
	{
		if (c.second->GetType() == "RENDERABLE")
		{
			continue;
		}

		if (c.second->IsEnabled() == false)
		{
			continue;
		}

		c.second->Render();
	}

	for each (std::pair<std::string, IActor*> a in Children)
	{
		if (a.second->IsEnabled())
		{
			a.second->Render();
		}
	}
}

IActor* Actor::AddChild(IActor *actor)
{
	if (actor)
	{
		std::string name = actor->GetName();

		if (name.size() > 0)
		{
			IActor *a = GetChild(name);

			if (a)
			{
				// TODO(Kai): Exists

				return a;
			}
			else
			{
				Actor *p = (Actor *)actor->GetParent();

				if (p)
				{
					p->Children.erase(name);
				}
				else
				{
					Scene->AddActor(actor);
					p = (Actor *)actor->GetParent();
					p->Children.erase(name);
				}

				actor->SetParent(this);
				actor->GetTransform()->SetParent(ActorTransform);
				actor->SetScene(Scene);
				actor->SetEnable(true);

				actor->Init();					

				Children[name] = actor;

				return actor;
			}
		}
		else
		{
			// TODO(Kai): ERROR LOG

			return NULL;
		}
	}
	else
	{
		// TODO(Kai): ERROR LOG

		return NULL;
	}
}

IActor * Actor::AddChild(const std::string &name)
{
	if (name.size() > 0)
	{
		IActor *a = GetChild(name);

		if (a)
		{
			// TODO(Kai): Exists

			return a;
		}
		else
		{
			Actor *actor = new Actor(name, new Transform());

			AddChild(actor);

			return actor;
		}
	}
	else
	{
		// TODO(Kai): ERROR LOG

		return NULL;
	}
}

IComponent* Actor::AddComponent(IComponent *comp)
{
	if (comp)
	{
		std::string name = comp->GetName();

		if (name.size() > 0)
		{
			IComponent *c = GetComponent(name);
			std::vector<IComponent *> *components = Scene->GetAllComponents(comp->GetType());

			if (!components)
			{
				if (Scene)
				{
					Scene->AddComponentList(comp);
				}
			}

			if (c)
			{
				return c;
			}
			else
			{
				Actor *o = (Actor*)comp->GetOwner();

				if (o)
				{
					o->Components.erase(name);
				}

				comp->SetOwner(this);
				comp->Init();
				comp->SetEnable(true);

				Components[name] = comp;
				Scene->AddComponent(comp);

				return comp;
			}
		}
		else
		{
			// TODO(Kai): ERROR LOG
			
			return NULL;
		}
	}
	else
	{
		// TODO(Kai): ERROR LOG

		return NULL;
	}
}

void Actor::RemoveChild(const std::string &name)
{
	if (name.size() > 0)
	{
		IActor *a = GetChild(name);

		if (a)
		{
			a->Destroy();
		
			if (a->GetTransform())
			{
				delete (a->GetTransform());
			}
		
			Children.erase(name);
		
			delete a;
			a = NULL;
		}
		else
		{
			// TODO(Kai): ERROR LOG
		}
	}
	else
	{
		// TODO(Kai): ERROR LOG
	}
}

void Actor::RemoveAllChildren()
{
	if (Children.size() > 0)
	{			
		for each (std::pair<std::string, IActor*> a in Children)
		{
			a.second->Destroy();

			if (a.second->GetTransform())
			{
				delete (a.second->GetTransform());
			}

			delete (a.second);
			a.second = NULL;
		}

		Children.clear();
	}
}

void Actor::RemoveComponent(const std::string &name)
{
	if (name.size() > 0)
	{
		IComponent *comp = GetComponent(name);

		if (comp)
		{
			comp->Destroy();

			Components.erase(name);
		
			delete comp;
			comp = NULL;
		}
		else
		{
			// TODO(Kai): ERROR LOG
		}
	}
	else
	{
		// TODO(Kai): ERROR LOG
	}
}

void Actor::RemoveAllComponents()
{
	for each (std::pair<std::string, IComponent*> c in Components)
	{
		c.second->Destroy();

		delete (c.second);

		c.second = NULL;
	}

	Components.clear();
}

IActor* Actor::GetChild(const std::string &name)
{
	if (name.size() > 0)
	{
		std::map<std::string, IActor*>::iterator it;

		it = Children.find(name);

		if (it != Children.end())
		{
			return it->second;
		}
		else
		{
			// TODO(Kai): NOT FOUND

			return NULL;
		}
	}
	else
	{
		// TODO(Kai): ERROR LOG
		
		return NULL;
	}
}

IComponent* Actor::GetComponent(const std::string &name)
{
	if (name.size() > 0)
	{
		std::map<std::string, IComponent*>::iterator it;

		it = Components.find(name);

		if (it != Components.end())
		{
			return it->second;
		}
		else
		{
			// TODO(Kai): NOT FOUND

			return NULL;
		}
	}
	else
	{
		// TODO(Kai): ERROR LOG

		return NULL;
	}
}

void Actor::Destroy()
{
	RemoveAllComponents();
	RemoveAllChildren();
}

uint32 Actor::GetChildrenCount() { return Children.size(); }
uint32 Actor::GetComponentCount() { return Components.size(); }