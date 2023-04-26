#pragma once

#include "IRenderingEngine.h"


class BasicRenderer : public IRenderingEngine
{
public:
	BasicRenderer() {}
	~BasicRenderer() {}

public:
	// NOTE(Kai): Override the pure virtual methods in RenderingEngine
	virtual void Init() override;

	virtual void RenderAll() override;
	virtual void RenderSingle(IComponent *comp) override;

	virtual void Destroy() override;

	virtual void Update() override {}
};
