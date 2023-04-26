#pragma once

#include "MeshRenderer.h"
#include <fstream>

struct Level
{
	std::vector<std::string> Data;
	std::vector<std::vector<vec3f>> Positions;

	int Width;
	int Height;

	Actor *Player;
	Actor *Base;

	vec3f CurrentPosMap;
	vec3f MoveDirection;
	vec3f PlayerPos;
	vec3f GoalPosition;
};

class LevelRenderer : public IComponent
{
public:
	LevelRenderer(Texture &tex) 
		: IComponent("LEVEL", "LEVEL RENDERER"), Block(tex)
	{}
	~LevelRenderer() {}

public:
	void Init() override 
	{
		CurrentLevel = 0;
		OnGoal = false;
		IsMoving = false;
		BlockSize = 40;
	}

	void Update() override 
	{	
		if (OnGoal)
		{
			Levels[CurrentLevel].Base->SetEnable(false); 
			
			CurrentLevel++;

			if (CurrentLevel >= Levels.size())
			{
				CurrentLevel = 0;
			}

			Levels[CurrentLevel].Base->SetEnable(true);

			IsMoving = false;
			OnGoal = false;

			Levels[CurrentLevel].Player->GetTransform()->SetPosition(vec3f(Levels[CurrentLevel].PlayerPos.Z * BlockSize, BlockSize, Levels[CurrentLevel].PlayerPos.X * BlockSize));
			Levels[CurrentLevel].CurrentPosMap = Levels[CurrentLevel].PlayerPos;
		}

		if (IsMoving == false)
		{
			if (InputManager::IsKeyUp(GLFW_KEY_UP))
			{
				while (Levels[CurrentLevel].CurrentPosMap.X > 0 && Levels[CurrentLevel].Data[Levels[CurrentLevel].CurrentPosMap.X - 1][Levels[CurrentLevel].CurrentPosMap.Z] != '1')
				{
					Levels[CurrentLevel].CurrentPosMap.X--;
				}

				IsMoving = true;

				Levels[CurrentLevel].MoveDirection = vec3f(0, 0, -1);
			}
			else if (InputManager::IsKeyUp(GLFW_KEY_DOWN))
			{
				while (Levels[CurrentLevel].CurrentPosMap.X < Levels[CurrentLevel].Height - 1 && Levels[CurrentLevel].Data[Levels[CurrentLevel].CurrentPosMap.X + 1][Levels[CurrentLevel].CurrentPosMap.Z] != '1')
				{
					Levels[CurrentLevel].CurrentPosMap.X++;
				}

				IsMoving = true;

				Levels[CurrentLevel].MoveDirection = vec3f(0, 0, 1);
			}
			else if (InputManager::IsKeyUp(GLFW_KEY_LEFT))
			{
				while (Levels[CurrentLevel].CurrentPosMap.Z > 0 && Levels[CurrentLevel].Data[Levels[CurrentLevel].CurrentPosMap.X][Levels[CurrentLevel].CurrentPosMap.Z - 1] != '1')
				{
					Levels[CurrentLevel].CurrentPosMap.Z--;
				}

				IsMoving = true;

				Levels[CurrentLevel].MoveDirection = vec3f(-1, 0, 0);
			}
			else if (InputManager::IsKeyUp(GLFW_KEY_RIGHT))
			{
				while (Levels[CurrentLevel].CurrentPosMap.Z < Levels[CurrentLevel].Width - 1 && Levels[CurrentLevel].Data[Levels[CurrentLevel].CurrentPosMap.X][Levels[CurrentLevel].CurrentPosMap.Z + 1] != '1')
				{
					Levels[CurrentLevel].CurrentPosMap.Z++;
				}

				IsMoving = true;

				Levels[CurrentLevel].MoveDirection = vec3f(1, 0, 0);
			}
		}		

		if (IsMoving && !OnGoal)
		{
			MovePlayer(vec3f(Levels[CurrentLevel].CurrentPosMap.Z * BlockSize, BlockSize, Levels[CurrentLevel].CurrentPosMap.X * BlockSize), Levels[CurrentLevel].MoveDirection);
		}
	}
	
	void Render() override {}
	void Destroy() override {}

	void AddLevel(char *path) 
	{
		Level level = {};

		std::ifstream file(path);

		std::string row;

		while (std::getline(file, row))
		{	
			level.Data.push_back(row);
		}

		level.Width = level.Data[0].size();
		level.Height = level.Data.size();

		Shader *shader = new DirectionalShader();
		Actor *base = new Actor("level" + std::to_string(Levels.size()), new Transform());
		Owner->GetScene()->AddActor(base);
		Owner->AddChild(base);

		level.Base = base;

		for (int j = level.Data.size() - 1; j >= 0; j--)
		{
			for (int i = 0; i < level.Data[0].size(); i++)
			{
				float x = i  * BlockSize;
				float z = j  * BlockSize;
				vec3f pos(x, 0, z);
				std::string name = "block";
				name += std::to_string(Owner->GetScene()->GetActorCount());
				Actor *a = new Actor(name, new Transform());
				Owner->GetScene()->AddActor(a);
				base->AddChild(a);

				MeshRenderer *mr;

				if (level.Data[j][i] == '1')
				{
					mr = new MeshRenderer("mr", "RENDERER",
										   MeshFactory::CreateCuboid(vec3f(BlockSize, BlockSize, BlockSize), true),
										   vec3f(0.2f, 0.2f, 0.2f), Block,
										   shader);

					MeshRenderer *mr1 = new MeshRenderer("mr1", "RENDERER",
						MeshFactory::CreateCuboid(vec3f(45, 45, 45), true),
						vec3f(1, 1, 1), Block,
						shader);

					Actor *a1 = new Actor(name + "_", new Transform());
					Owner->GetScene()->AddActor(a1);
					base->AddChild(a1);
					a1->AddComponent(mr1);

					pos.Y += BlockSize;
					a1->GetTransform()->SetPosition(vec3f(pos.X, 0, pos.Z));
				}
				else if (level.Data[j][i] == '2' && level.Player == NULL)
				{
					mr = new MeshRenderer("mr", "RENDERER",
										   MeshFactory::CreateCuboid(vec3f(BlockSize, BlockSize, BlockSize), true),
										   vec3f(1, 1, 0), Block,
										   shader);

					MeshRenderer *mr1 = new MeshRenderer("mr1", "RENDERER",
						MeshFactory::CreateCuboid(vec3f(BlockSize, BlockSize, BlockSize), true),
						vec3f(1, 1, 1), Block,
						shader);

					Actor *a1 = new Actor(name + "_", new Transform());
					Owner->GetScene()->AddActor(a1);
					base->AddChild(a1);
					a1->AddComponent(mr1);

					pos.Y += BlockSize;
					a1->GetTransform()->SetPosition(vec3f(pos.X, 0, pos.Z));

					level.PlayerPos.X = j;
					level.PlayerPos.Z = i;

					level.CurrentPosMap.X = j;
					level.CurrentPosMap.Z = i;

					level.Player = a;
				}
				else if (level.Data[j][i] == '3')
				{
					mr = new MeshRenderer("mr", "RENDERER",
										   MeshFactory::CreateCuboid(vec3f(BlockSize + 0.5f, BlockSize + 0.5f, BlockSize + 0.5f), true),
										   vec3f(1, 0, 0), Block,
										   shader);

					level.GoalPosition.X = j;
					level.GoalPosition.Z = i;
				}
				else
				{
					mr = new MeshRenderer("mr", "RENDERER",
										  MeshFactory::CreateCuboid(vec3f(BlockSize, BlockSize, BlockSize), true),
										  vec3f(1, 1, 1), Block,
										  shader);
				}

				a->AddComponent(mr);

				a->GetTransform()->SetPosition(pos);

			}
		}
		
		level.Base->SetEnable(false);

		Levels.push_back(level);

		Levels[CurrentLevel].Base->SetEnable(true);
	}

private:
	void MovePlayer(vec3f &to, vec3f &dir) 
	{
		float dist = Vec3::Length(Levels[CurrentLevel].Player->GetTransform()->GetPosition() - to);

		if (dist < 0.01f)
		{
			IsMoving = false;
			
			if (Levels[CurrentLevel].CurrentPosMap == Levels[CurrentLevel].GoalPosition)
			{
				OnGoal = true;
			}

			return;
		}
		else
		{
			Levels[CurrentLevel].Player->GetTransform()->Translate(5, dir);
		}
	}

private:
	std::vector<Level> Levels;
	Texture Block;
	int CurrentLevel;
	float BlockSize;
	
	bool IsMoving;
	bool OnGoal;
};
