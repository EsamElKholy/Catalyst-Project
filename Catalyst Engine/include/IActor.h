#pragma once

#include <string>

class IComponent;
class Transform;
class SceneManager;

class IActor
{
public:
	IActor(std::string name = "", Transform *trans = 0, IActor *parent = 0)
		: Name(name), ActorTransform(trans), Parent(parent)
	{}

	virtual ~IActor() {}

public:
	virtual void Init() = 0;

	// NOTE(Kai): Update this actor (call of its components' update method)
	virtual void Update() = 0;

	// NOTE(Kai): Render this actor (call of its components' render method)
	virtual void Render() = 0;

	// NOTE(Kai): Update this actor and all its children
	virtual void UpdateAll() = 0;

	// NOTE(Kai): Render this actor and all its children
	virtual void RenderAll() = 0;

	virtual IActor* AddChild(IActor *actor) = 0;
	virtual IActor* AddChild(const std::string &name) = 0;
	virtual IComponent* AddComponent(IComponent *comp) = 0;

	virtual void RemoveChild(const std::string &name) = 0;
	virtual void RemoveAllChildren() = 0;

	virtual void RemoveComponent(const std::string &name) = 0;
	virtual void RemoveAllComponents() = 0;

	virtual IActor* GetChild(const std::string &name) = 0;
	virtual IComponent* GetComponent(const std::string &name) = 0;

	virtual void Destroy() = 0;

public:
	inline void SetName(const std::string &name) { Name = name; }
	inline void SetTransform(Transform *trans) { ActorTransform = trans; }
	inline void SetParent(IActor *parent) { Parent = parent; }
	inline void SetEnable(bool enable) { Enabled = enable; }
	inline void SetScene(SceneManager *scene) { Scene = scene; }

	inline std::string GetName() { return Name; }
	inline Transform* GetTransform() { return ActorTransform; }
	inline IActor* GetParent() { return Parent; }
	inline bool IsEnabled() { return Enabled; }
	inline SceneManager* GetScene() { return Scene; }

protected:
	SceneManager *Scene;
	IActor *Parent;
	Transform *ActorTransform;
	std::string Name;
	bool Enabled;
};