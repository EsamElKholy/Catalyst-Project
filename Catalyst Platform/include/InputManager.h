#pragma once

#include <GLFW\glfw3.h>
#include <Catalyst.h>

class PlatformInputManager
{
public:
	~PlatformInputManager() {}
private:
	PlatformInputManager() {}
	PlatformInputManager(const PlatformInputManager &im) = default;
	PlatformInputManager& operator=(const PlatformInputManager &other) = default;

public:
	static void Init();
	static void Key_Callback(GLFWwindow *window, int key, int scancode, int action, int mode);
	static void Button_Callback(GLFWwindow *window, int button, int action, int mode);
	static void Mouse_Position_Callback(GLFWwindow *window, double xPos, double yPos);
	static void Scroll_Callback(GLFWwindow *window, double xOffset, double yOffset);

private:
	static bool MouseMove;
};
