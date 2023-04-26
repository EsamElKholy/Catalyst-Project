#include <GLSandbox.h>
#include <MeshRenderer.h>
#include <PlayerController.h>
#include <LineSketcher.h>
#include <SineRenderer.h>
#include <LevelRenderer.h>

void GLSandbox::Init()
{
	InitializeScene();
	/*
	
	Fractals();
	MovingSine();

	*/
	//RotatingCircle();
	
	//LineSketch();
	CreateLevel();
	
	//CreateShape();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void GLSandbox::Render()
{
	Scene->Render();
}

void GLSandbox::Update()
{
	Scene->Update(); 
}

void GLSandbox::Destroy()
{
	Scene->Destroy();
}

void GLSandbox::CreateShape()
{
	Actor *a = new Actor("kai", new Transform());
	Scene->AddActor(a);

	MeshRenderer *mr = new MeshRenderer("r22", "RENDERABLE"
		//, MeshFactory::CreateQuad(vec3f(800, 800, 0.0f), "XY", "front", 0)
		//, MeshFactory::CreateWiredCuboid(vec3f(180, 180, 180.0f), 0)
		//, MeshFactory::CreateTetrahedronSphere(100, 5, 0)
		, MeshFactory::CreateCuboid(vec3f(180, 180, 180.0f), 0)
		//, MeshFactory::CreateSphere(vec3f(150.1f, 150.1f, 150.1f), 2, 0)
		//, MeshFactory::CreateCylinder(150, vec3f(50.1f, 50.1f, 50.1f), vec3f(50.1f, 50.1f, 50.1f), 10, 0)
		, vec3f(0, 1, 1)
		//, Resources.GetTexture("container")
		//, new BasicShader("Resources/Shaders/fractal shader.vert", "Resources/Shaders/fractal shader.frag"));
		, new BasicShader());

	a->AddComponent(mr);
}

void GLSandbox::LineSketch()
{
	Actor *a = new Actor("kai", new Transform());
	Scene->AddActor(a);
	LineSketcher *ls = new LineSketcher("ls", "LineSketcher", vec3f(0, 1, 1), new BasicShader());
	a->AddComponent(ls);
}

void GLSandbox::RotatingCircle()
{
	Resources.AddTexture("Resources/Textures/volt.png", "zombi");
	Resources.AddTexture("Resources/Textures/container2.png", "container");
	Resources.AddTexture("Resources/Textures/container2_specular.png", "container_specular");
	Resources.AddMaterial("container", Material{ Resources.GetTexture("container"), Resources.GetTexture("container_specular") ,1.5f });
	//Resources.AddMaterial("world", Material{ Resources.GetTexture("world"), Resources.GetTexture("world") ,1.5f });
	SpriteRenderer *sr = new SpriteRenderer("sr", "RENDERABLE", Resources.GetTexture("zombi"), vec3f(100, 100, 0), vec3f(1, 1, 1), new DirectionalShader());
	Animator *animator = new Animator("animator", sr);
	std::vector<std::pair<uint32, float>> frames;
	for (int i = 5; i < 10; i++)
	{
		frames.push_back(std::make_pair(i, 3.0f / 60.0f));
	}
	animator->AddClip("zombi walk", &AnimationClip(Resources.GetTexture("zombi"), 5, 2, &frames, true));
	
	LightmapShader *shader = new LightmapShader();

	Actor *a = new Actor("kai", new Transform());
	Actor *a1 = new Actor("kai1", new Transform());
	Actor *a2 = new Actor("kai2", new Transform());
	Actor *a3 = new Actor("kai3", new Transform());
	Scene->AddActor(a);
	Scene->AddActor(a1);
	Scene->AddActor(a2);
	Scene->AddActor(a3);

	MeshRenderer *mr00 = new MeshRenderer("r22", "RENDERABLE"
		//, MeshFactory::CreateQuad(vec3f(800, 800, 0.0f), "XY", "front", 0)
		//, MeshFactory::CreateWiredCuboid(vec3f(180, 180, 180.0f), 0)
		//, MeshFactory::CreateCuboid(vec3f(180, 180, 180.0f), 0)
		//, MeshFactory::CreateCircle(vec2f(180, 180), 100, 0)
		, MeshFactory::CreateSphere(vec3f(50, 55, 50), 4, 0)
		//, MeshFactory::CreateTetrahedronSphere(150, 7, 0)
		//, MeshFactory::CreateMeshOBJ("Resources/Models/box_stack.obj", 0)
		//, MeshFactory::CreateCylinder(150, vec3f(50.1f, 50.1f, 50.1f), vec3f(50.1f, 50.1f, 50.1f), 10, 0)
		//, vec3f(1, 1, 1) 
		, vec3f(0, 0, 1)
		//, Resources.GetTexture("container")
		//, new BasicShader());
		, new DirectionalShader(), true);

	MeshRenderer *mr01 = new MeshRenderer("r22", "RENDERABLE"
		//, MeshFactory::CreateQuad(vec3f(800, 800, 0.0f), "XY", "front", 0)
		//, MeshFactory::CreateWiredCuboid(vec3f(180, 180, 180.0f), 0)
		//, MeshFactory::CreateCuboid(vec3f(180, 180, 180.0f), 0)
		//, MeshFactory::CreateCircle(vec2f(180, 180), 100, 0)
		, MeshFactory::CreateSphere(vec3f(100, 100, 100), 4, 0)
		//, MeshFactory::CreateTetrahedronSphere(150, 7, 0)
		//, MeshFactory::CreateMeshOBJ("Resources/Models/box_stack.obj", 0)
		//, MeshFactory::CreateCylinder(150, vec3f(50.1f, 50.1f, 50.1f), vec3f(50.1f, 50.1f, 50.1f), 10, 0)
		//, vec3f(1, 1, 1) 
		, vec3f(1, 1, 0)
		//, Resources.GetTexture("container")
		//, new BasicShader());
		, new DirectionalShader(), true);

	//mr->SetFog(new FogShader(), vec3f(0.15, 0.15, 0.15), 1, 850, 0.002);

	LightsRenderer *mr1 = new LightsRenderer("r1", "RENDERABLE"
		, MeshFactory::CreateCuboid(vec3f(5000, 10, 5000), true, 0, 0, vec2f(100, 100))
		, vec3f(1, 1, 1)
		, shader, Resources.GetMaterial("container"));

	LightsRenderer *mr2 = new LightsRenderer("r1", "RENDERABLE"
		, MeshFactory::CreateSphere(vec3f(155.1f, 1055.1f, 150), 4, 0)
		//, MeshFactory::CreateMeshOBJ("Resources/Models/sph.obj", 0)
		, vec3f(1, 1, 1)
		, shader, Resources.GetMaterial("container"));

	DirectionalLight directional =
		DirectionalLight
	{
		vec3f(-10, -5, -5),
		vec3f(1, 1, 1),
		vec3f(1, 1, 1),
		vec3f(1, 1, 1),
		0.007f
	};

	PointLight point =
		PointLight
	{
		vec3f(0, -15),
		vec3f(0, 1, 0.7f),
		vec3f(0, 1, 0.7f),
		vec3f(0, 1, 0.7f),
		Attenuation{ 0.1f, 0.22f, 0.20f },
		0.07f
	};

	mr1->AddPointLight(point);
	mr2->AddPointLight(point);
	mr1->SetDirectionalLight(directional);
	mr2->SetDirectionalLight(directional);

	a->AddComponent(mr1);
	a->AddComponent(mr2);

	a1->AddComponent(mr01);
	a2->AddComponent(mr00);

	a3->AddComponent(sr);
	a3->AddComponent(animator);

	//a1->GetTransform()->SetScale(vec3f(10, 10, 0));
	a2->GetTransform()->SetPosition(vec3f(-720, 0, 0));
	a3->GetTransform()->SetPosition(vec3f(0, 400, 0));
	a->GetTransform()->SetPosition(vec3f(0, -120, 0));
	PlayerController *pc = new PlayerController("pc", "PlayerController");
	a1->AddComponent(pc);
	a->AddComponent(new FPSCamera());

	a1->AddChild(a2);
}

void GLSandbox::Fractals()
{
	Actor *a = new Actor("kai", new Transform());
	Scene->AddActor(a);

	MeshRenderer *mr = new MeshRenderer("r22", "RENDERABLE"
		, MeshFactory::CreateQuad(vec3f(800, 800, 0.0f), "XY", "front", 0)
		//, MeshFactory::CreateWiredCuboid(vec3f(180, 180, 180.0f), 0)
		//, MeshFactory::CreateCuboid(vec3f(180, 180, 180.0f), 0)
		//, MeshFactory::CreateSphere(vec3f(150.1f, 150.1f, 150.1f), 2, 0)
		//, MeshFactory::CreateCylinder(150, vec3f(50.1f, 50.1f, 50.1f), vec3f(50.1f, 50.1f, 50.1f), 10, 0)
		, vec3f(0, 0, 0)
		//, Resources.GetTexture("container")
		, new BasicShader("Resources/Shaders/fractal shader.vert", "Resources/Shaders/fractal shader.frag"));
		//, new BasicShader());

	a->AddComponent(mr);
}

void GLSandbox::CreateLevel()
{
	Resources.AddTexture("Resources/Textures/container2.png", "container");
	
	Actor *a = new Actor("kai", new Transform());
	Scene->AddActor(a);

	LevelRenderer *lr = new LevelRenderer(Resources.GetTexture("container"));

	a->AddComponent(lr);

	lr->AddLevel("Resources/Levels/level1.txt");
	lr->AddLevel("Resources/Levels/level2.txt");
	lr->AddLevel("Resources/Levels/level3.txt");
	a->AddComponent(new FPSCamera());
	//a->GetTransform()->SetPosition(vec3f(0, -100, 0));
}

void GLSandbox::MovingSine()
{
	Actor *a = new Actor("kai", new Transform());
	Scene->AddActor(a);
	SineRenderer *sr = new SineRenderer("sr", "SineRenderer", vec3f(0, 1, 1), new SineShader());
	a->AddComponent(sr);
}

void GLSandbox::InitializeScene()
{
	Scene = new SceneManager();
	Scene->Init(new BasicRenderer());

	Camera *cam = new Camera();

	//lcam->SetProjectionData(45.0f, 720, 1280, 0.1f, 10000.0f);
	cam->SetProjectionData(45.0f, 800, 600, 0.1f, 10000.0f);
	//cam->SetViewData(vec3f(0, 0, 200), vec3f(0, 0, -1), vec3f(0, -1, 0), vec3f(1, 1, 1), "ORTHO");
	cam->SetViewData(vec3f(41.5794907, 500.067108, 340.855225), vec3f(0.140250862, -0.876222849, -0.461045772), vec3f(0, -1, 0), vec3f(0.8f, 0.8f, 0.8f), "PERSPECTIVE");
	cam->SetLock(false);
	Scene->SetMainCamera(cam);
}