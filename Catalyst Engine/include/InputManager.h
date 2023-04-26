#pragma once

#include <cinder\CinderMath.h>

using namespace Cinder::Math;

class InputManager
{
public:
	~InputManager() {}

private:
	InputManager() {}

public:
	static void Init();

	static bool IsKeyDown(int key);

	static bool IsKeyUp(int key);

	static bool IsButtonDown(int button);

	static bool IsButtonUp(int button);

	static vec2f GetMousePos();
	static vec2f GetMousePosOffset();

	static vec2f GetMousePosNDC();

	static float GetWheelRotation();
	static long GetWheelTimeStamp();

	static vec2f GetWindowSize();

	static void SetKeyUp(int key, bool state);

	static void SetKeyDown(int key, bool state);

	static void SetButtonUp(int button, bool state);

	static void SetButtonDown(int button, bool state);

	static void SetMousePos(vec2f &pos);

	static void SetWheelRotation(float rot, long timeStamp);

	static void SetWindowSize(vec2f &size);

	static vec3f GetProjectedMousePos(mat4f &proj, mat4f &view, vec3f &pos, bool ortho);

	static bool IsCursorLocked();
	static void SetCursorLock(bool lock);

private:
	static vec2f MousePos;
	static vec2f MousePosNDC;
	static vec2f MousePosOffset;

	static float WheelRotation;
	static long WheelTimeStamp;

	static vec2f WindowSize;

	static bool MouseMoved;
	static bool CursorLocked;

	static bool KeysDown[500];
	static bool KeysUp[500];

	static bool ButtonsDown[4];
	static bool ButtonsUp[4];

	static bool FirstMouse;
};
