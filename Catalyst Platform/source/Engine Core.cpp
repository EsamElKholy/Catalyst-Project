#include <Engine Core.h>

EngineCore::EngineCore(int width, int height, char * title, int ups, IApplication *app)
{
	Init(width, height, title, ups, app);
}

void EngineCore::Init(int width, int height, char * title, int ups, IApplication *app)
{
	IsRunning = false;
	UPS = ups;

	WindowManager::CreateOpenGLWindow(width, height, title);
	//WindowManager::CreateVulkanWindow(width, height, title);

	PlatformInputManager::Init();

	App = NULL;

	if (app)
	{
		App = app;
		App->Init();
	}
	else
	{
		std::cout << "APP NOT FOUND!!\n";
	}
}

void EngineCore::Run()
{
	if (!IsRunning)
	{
		IsRunning = true;
		MainLoop();
	}
}

void EngineCore::Stop()
{
	IsRunning = false;
}

void EngineCore::MainLoop()
{
	double lastTime = Timer::GetTime();
	double timer = Timer::GetTime();
	double delta = 0;

	int updates = 0;
	int frames = 0;

	while (IsRunning)
	{
		double currentTime = Timer::GetTime();
		delta += (currentTime - lastTime) * (float)UPS;
		lastTime = currentTime;

		while (delta >= 1.0)
		{
			if (WindowManager::ShouldClose())
			{
				Stop();
			}

			Update();
			updates++;
			delta--;
		}

		WindowManager::SetClearColor(0.15, 0.15, 0.15, 0);
		
		Render();

		frames++;

		if (Timer::GetTime() - timer >= 1.0) 
		{
			timer = Timer::GetTime();
			std::cout << "FPS: " << frames << ", UPS:" << updates /*<< ", delta: " << 1.0f / (float)UPS*/ << "\n";
			frames = 0;
			updates = 0;
		}
	}

	WindowManager::Destroy();
}

void EngineCore::Update()
{
	WindowManager::Update();

	if (App)
	{
		App->Update();
	}		
}

void EngineCore::Render()
{
	WindowManager::ClearWindow();

	if (App)
	{
		App->Render();
	}

	WindowManager::Render();
}