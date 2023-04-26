#pragma once

#include <cinder\CinderMath.h>
#include <unordered_map>
#include <vector>
#include <map>

class IActor;
class IComponent;
class IRenderingEngine;
class Camera;

/*
class SceneManager
{
public:
	SceneManager() {}
	~SceneManager() {}

public:
	void Init();

	// NOTE(Kai): Call all actors' 'Update' methods 
	void Update();

	// NOTE(Kai): Call all actors' 'Render' methods
	void Render();

	// NOTE(Kai): Destroy all systems if there is any 
	//			(currently there's just rendering engine) and destroy all actors
	void Destroy();

	// NOTE(Kai): This method will access all actors
	//			, get all components with the specified type and push then in a queue
	std::vector<IComponent*>* GetAllComponents(ComponentType type);

	// NOTE(Kai): Add child to a parent
	IActor* AddActor(char *name, char *parent);

	void RemoveComponentType(ComponentType type);
	void RemoveComponent(IComponent *comp);
	void RemoveAllComponents(ComponentType name);
public:
	inline void SetRenderingEngine(IRenderingEngine *engine);

private:
	// NOTE(Kai): Empty Actor without components, any new add actor will be added as a child to this actor
	IActor *Root;

	// NOTE(Kai): Rendering system 
	IRenderingEngine *Renderer;

	std::map<ComponentType, std::vector<IComponent*>> AllComponents;
};

*/

class SceneManager
{
public:
	SceneManager()
	{

	}

	~SceneManager() {}

public:
	void Init(IRenderingEngine *rengine);

	IActor* AddActor(const std::string &name);
	IActor* AddActor(IActor *actor);

	IActor* GetActor(const std::string &name);
	void RemoveActor(const std::string &name);
	
	void AddComponentList(IComponent *component);
	IComponent* AddComponent(IComponent *component);

	inline Camera* GetMainCamera() { return MainCamera; }
	inline void SetMainCamera(Camera *cam) { MainCamera = cam; }

	inline int GetActorCount() { return Actors.size(); }
	inline IRenderingEngine* GetRenderingEngine() { return REngine; }
	inline void SetRenderingEngine(IRenderingEngine *rengine) { REngine = rengine; }

	void Render();
	void Update();
	void Destroy();

	std::vector<IComponent *>* GetAllComponents(std::string type);

private:
	IActor* CreateObject(const std::string &name);

	void RemoveComponent(IComponent *comp);
	void RemoveAllComponents(const std::string &name);
	void RemoveComponentList(const std::string &type);

private:
	std::unordered_map<std::string, IActor *> Actors;
	std::unordered_map<std::string, std::vector<IComponent*>> AllComponents;

	IRenderingEngine *REngine;
	
	IActor *Root;
	Camera *MainCamera;
};