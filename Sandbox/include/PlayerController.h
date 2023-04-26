#pragma once

#include <Catalyst.h>
#include <InputManager.h>

class PlayerController : public IComponent
{
public:
	PlayerController(const std::string &name, const std::string &type)//, Actor *player)
		: IComponent(name, type)//, Player(player)
	{}

	~PlayerController() {}

public:
	void Init() override 
	{
		Speed = 0.3;
	}

	void Update() override 
	{
		Owner->GetTransform()->Rotate(Speed, vec3f(0, 1, 0));
	}

	void Render() override {}
	void Destroy() override {}

private:
	Actor *Player;
	float Speed;
};