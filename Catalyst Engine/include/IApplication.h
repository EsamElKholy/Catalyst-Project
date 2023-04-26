#pragma once

#include "ResourceCache.h"
#include "SceneManager.h"

class IApplication
{
public:
	IApplication() {}
	virtual ~IApplication() {}

public:
	void PRE_INIT() 
	{
		
	}
	virtual void Init() = 0;
	virtual void Render() = 0;
	virtual void Update() = 0;
	virtual void Destroy() = 0;

public:
	inline SceneManager* GetSceneManager() { return Scene; }
	inline void SetSceneManager(SceneManager *scene) { Scene = scene; }
	inline ResourceCache& GetResourceCache() { return Resources; }

protected:
	SceneManager *Scene;
	ResourceCache Resources;
};
