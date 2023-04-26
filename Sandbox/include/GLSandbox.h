#pragma once

#include <Catalyst.h>


class GLSandbox : public IApplication
{
public:
	GLSandbox() {}
	~GLSandbox()
	{
		Destroy();
	}

public:
	virtual void Init() override;

	virtual void Render() override;

	virtual void Update() override;

	virtual void Destroy() override;

private:
	void CreateShape();
	void LineSketch();
	void RotatingCircle();
	void Fractals();
	void CreateLevel();
	void MovingSine();

private:
	void InitializeScene();
};