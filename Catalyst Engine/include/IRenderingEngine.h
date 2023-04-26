#pragma once

class SceneManager;
class IComponent;

class IRenderingEngine
{
public:
	IRenderingEngine() {}
	virtual ~IRenderingEngine() {}

public:
	// NOTE(Kai): Pure virtual methods

	virtual void Init() = 0;

	// NOTE(Kai): Render all components (Generally by getting a queue of components with 
	//			'RENDERABLE_COMPONENT' type)
	virtual void RenderAll() = 0;

	// NOTE(Kai): Render a single component with 'RENDERABLE_COMPONENT' type
	virtual void RenderSingle(IComponent *comp) = 0;

	virtual void Destroy() = 0;

	virtual void Update() = 0;

public:
	inline SceneManager* GetOwner() { return Owner; }
	inline void SetOwner(SceneManager *owner) { Owner = owner; }

protected:
	SceneManager *Owner;
};
