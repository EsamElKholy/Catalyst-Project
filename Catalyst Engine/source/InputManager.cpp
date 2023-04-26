#include <InputManager.h>

bool InputManager::KeysDown[500];
bool InputManager::KeysUp[500];

bool InputManager::ButtonsDown[4];
bool InputManager::ButtonsUp[4];

bool InputManager::FirstMouse;
bool InputManager::CursorLocked;
vec2f InputManager::MousePos;
vec2f InputManager::MousePosNDC;
vec2f InputManager::MousePosOffset;

float InputManager::WheelRotation;
long InputManager::WheelTimeStamp;

vec2f InputManager::WindowSize;

void InputManager::Init() {}

bool InputManager::IsKeyDown(int key)
{
	return KeysDown[key];
}

bool InputManager::IsKeyUp(int key)
{
	bool state = KeysUp[key];

	KeysUp[key] = false;

	return state;
}

bool InputManager::IsButtonDown(int button)
{
	return ButtonsDown[button];
}

bool InputManager::IsButtonUp(int button)
{
	bool state = ButtonsUp[button];

	ButtonsUp[button] = false;

	return state;
}

vec2f InputManager::GetMousePos()
{
	return MousePos;
}

vec2f InputManager::GetMousePosOffset()
{
	return MousePosOffset;
}

vec2f InputManager::GetMousePosNDC()
{
	float x = ((2.0f * MousePos[0]) / WindowSize.X) - 1.0f;
	float y = 1.0f - ((2.0f * MousePos[1]) / WindowSize.Y);

	MousePosNDC[0] = x;
	MousePosNDC[1] = y;

	return MousePosNDC;
}

float InputManager::GetWheelRotation()
{
	return WheelRotation;
}

long InputManager::GetWheelTimeStamp()
{
	return WheelTimeStamp;
}

vec2f InputManager::GetWindowSize()
{
	return WindowSize;
}

void InputManager::SetKeyUp(int key, bool state)
{
	KeysUp[key] = state;
}

void InputManager::SetKeyDown(int key, bool state)
{
	KeysDown[key] = state;
}

void InputManager::SetButtonUp(int button, bool state)
{
	ButtonsUp[button] = state;
}

void InputManager::SetButtonDown(int button, bool state)
{
	ButtonsDown[button] = state;
}

void InputManager::SetMousePos(vec2f &pos)
{
	MousePos = pos;
}

void InputManager::SetWheelRotation(float rot, long timeStamp)
{
	WheelRotation = rot;
	WheelTimeStamp = timeStamp;
}

void InputManager::SetWindowSize(vec2f &size)
{
	WindowSize = size;
}

vec3f InputManager::GetProjectedMousePos(mat4f &proj, mat4f &view, vec3f &pos, bool ortho)
{
	vec2f mousePos_p = GetMousePosNDC();

	if (ortho)
	{
		vec4f mouseRay = vec4f(mousePos_p[0], mousePos_p[1], -pos.Z, 1);

		mat4f projInv = proj.Inverse();

		mouseRay = projInv * mouseRay;
		mouseRay.W = 0.0f;
		mouseRay.Z = -pos.Z;

		vec4f ray_world = view.Inverse() * mouseRay;

		vec3f resPos = vec3f(ray_world.X, ray_world.Y, ray_world.Z);

		return resPos;
	}
	else
	{
		vec4f mouseRay = vec4f(mousePos_p[0], mousePos_p[1], -1, 1);

		mat4f projInv = proj.Inverse();

		mouseRay = projInv * mouseRay;
		mouseRay.W = 0.0f;
		mouseRay.Z = -1.0f;

		vec4f ray_world = view.Inverse() * mouseRay;

		vec3f resPos = vec3f(ray_world.X, ray_world.Y, ray_world.Z);

		resPos = Vec3::Normalize(resPos);

		return resPos;
	}
}

bool InputManager::IsCursorLocked()
{
	return CursorLocked;
}

void InputManager::SetCursorLock(bool lock)
{
	CursorLocked = lock;
}
