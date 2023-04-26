#pragma once

#include "Camera.h"
#include "IComponent.h"
#include "Actor.h"
#include "SceneManager.h"
#include "InputManager.h"
#include <GLFW\glfw3.h>
#include <iostream>

class FPSCamera : public IComponent
{
public:
	FPSCamera() 
		: IComponent("fps cam", "CAMERA CONTROLLER")
	{
		Sensitivity = 0.09f;
		Speed = 200.0f;
	}
	
	~FPSCamera() {}

public:
	virtual void Init() override {}
	virtual void Update() override 
	{
		float cameraSpeed = Speed * (1.0f / 60.0f);
		Camera *cam = Owner->GetScene()->GetMainCamera();

		if (InputManager::IsKeyDown(GLFW_KEY_W))
		{
			cam->SetPosition(cam->GetPosition() + (cameraSpeed * cam->GetTarget()));
		}

		if (InputManager::IsKeyDown(GLFW_KEY_S))
		{
			cam->SetPosition(cam->GetPosition() - (cameraSpeed * cam->GetTarget()));
		}

		if (InputManager::IsKeyDown(GLFW_KEY_A))
		{
			cam->SetPosition(cam->GetPosition() - (cameraSpeed * Vec3::Normalize(Vec3::Cross(cam->GetUp(), cam->GetTarget()))));
		}

		if (InputManager::IsKeyDown(GLFW_KEY_D))
		{
			cam->SetPosition(cam->GetPosition() + (cameraSpeed * Vec3::Normalize(Vec3::Cross(cam->GetUp(), cam->GetTarget()))));
		}

		if (InputManager::IsKeyUp(GLFW_KEY_M))
		{
			MouseMove = !MouseMove;
		}
		if (MouseMove)
		{
			vec2f mousePos = InputManager::GetMousePos();

			if (mousePos.X != OldMousePos.X || mousePos.Y != mousePos.Y)
			{
				float xAxis = mousePos.X - OldMousePos.X;
				float yAxis = mousePos.Y - OldMousePos.Y;
				Yaw = std::fmod(Yaw + (xAxis * Sensitivity), 360.0f);
				Pitch += -yAxis * Sensitivity;

				if (Pitch < -89.0f)
				{
					Pitch = -89.0f;
				}

				if (Pitch > 89.0f)
				{
					Pitch = 89.0f;
				}

				vec3f newTarget;

				newTarget.X = cos(ToRadians(Pitch)) * cos(ToRadians(Yaw));
				newTarget.Y = sin(ToRadians(Pitch));
				newTarget.Z = cos(ToRadians(Pitch)) * sin(ToRadians(Yaw));
				//std::cout << Pitch << " " << Yaw << std::endl;
				cam->SetTarget(newTarget);
			}	

			OldMousePos = mousePos;
		}
	}

	virtual void Render() override {}
	virtual void Destroy() override {}

private:
	vec2f OldMousePos;
	float Sensitivity;
	float Speed;
	float Yaw;
	float Pitch;
	bool MouseMove;
};
