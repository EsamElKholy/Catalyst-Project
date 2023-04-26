#pragma once

#include "IActor.h"
#include <cinder\CinderMath.h>
#include <map>

class Actor : public IActor
{
public:
	Actor(std::string name = "", Transform *trans = NULL, IActor *parent = NULL)
		: IActor(name, trans, parent)
	{}

	~Actor()
	{}

public:
	virtual void Init() override;

	virtual void Update() override;
	virtual void Render() override;

	virtual void UpdateAll() override;
	virtual void RenderAll() override;

	virtual IActor* AddChild(IActor *actor) override;
	virtual IActor* AddChild(const std::string &name) override;
	virtual IComponent* AddComponent(IComponent *comp) override;

	virtual void RemoveChild(const std::string &name) override;
	virtual void RemoveAllChildren() override;

	virtual void RemoveComponent(const std::string &name) override;
	virtual void RemoveAllComponents() override;

	virtual IActor* GetChild(const std::string &name) override;
	virtual IComponent* GetComponent(const std::string &name) override;

	virtual void Destroy() override;

	std::map<std::string, IComponent*>& GetAllComponents() { return Components; }

	uint32 GetChildrenCount();
	uint32 GetComponentCount();

private:
	std::map<std::string, IComponent*> Components;
	std::map<std::string, IActor*> Children;
};
