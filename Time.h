#pragma once

#include <GLFW/glfw3.h>
#include <format>
#include <string>

class Time
{
	public:
	double prevTime = 0.0;
	double crntTime = 0.0;
	float deltaTime = 0.0;
	unsigned int counter = 0;

	void UpdateClock() 
	{
		crntTime = glfwGetTime();
		deltaTime = crntTime - prevTime;
		counter++;
	}

	void FPS_window(GLFWwindow* window)
	{
		if (deltaTime >= 1.0 / 30.0)
		{
			float fpsValue = (1.0f / deltaTime) * counter;
			std::string newTitle = std::format("Monkudox {:.1f} FPS", fpsValue);
			glfwSetWindowTitle(window, newTitle.c_str());

			prevTime = crntTime;
			counter = 0;
		}
	}
};
