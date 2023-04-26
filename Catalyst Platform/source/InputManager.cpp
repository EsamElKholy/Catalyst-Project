#include <InputManager.h>
#include <WindowManager.h>

bool PlatformInputManager::MouseMove;

void PlatformInputManager::Init()
{
	GLFWwindow *window = WindowManager::GetWindow();

	glfwSetKeyCallback(window, Key_Callback);
	glfwSetMouseButtonCallback(window, Button_Callback);
	glfwSetCursorPosCallback(window, Mouse_Position_Callback);
	glfwSetScrollCallback(window, Scroll_Callback);
}

void PlatformInputManager::Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
		return;
	}

	if (action == GLFW_RELEASE)
	{
		InputManager::SetKeyUp(key, true);
		InputManager::SetKeyDown(key, false);
	}

	if (action == GLFW_PRESS)
	{
		InputManager::SetKeyUp(key, false);
		InputManager::SetKeyDown(key, true);
	}

	if (InputManager::IsKeyUp(GLFW_KEY_M))
	{
		MouseMove = !MouseMove;

		InputManager::SetCursorLock(MouseMove);

		if (MouseMove)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		InputManager::SetKeyUp(key, true);
		InputManager::SetKeyDown(key, false);
	}
}

void PlatformInputManager::Button_Callback(GLFWwindow* window, int button, int action, int mode)
{
	if (action == GLFW_RELEASE)
	{
		InputManager::SetButtonUp(button, true);
		InputManager::SetButtonDown(button, false);
	}

	if (action == GLFW_PRESS)
	{
		InputManager::SetButtonUp(button, false);
		InputManager::SetButtonDown(button, true);
	}
}

void PlatformInputManager::Mouse_Position_Callback(GLFWwindow* window, double xPos, double yPos)
{
	InputManager::SetMousePos(vec2f(xPos, yPos));
}

void PlatformInputManager::Scroll_Callback(GLFWwindow *window, double xOffset, double yOffset)
{
	InputManager::SetWheelRotation(yOffset, 0);
}