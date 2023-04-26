#pragma once

#include <Catalyst.h>
#include <WindowManager.h>
#include <InputManager.h>
#include "Timer.h"

class EngineCore 
{
public:
	EngineCore(int width, int height, char *title, int ups, IApplication *app);
	~EngineCore() {}

public:
	void Init(int width, int height, char *title, int ups, IApplication *app);

	void Run();
	void Stop();

	void MainLoop();

	void Update();
	void Render();

private:
	IApplication *App;
	float UPS;
	bool IsRunning;
};