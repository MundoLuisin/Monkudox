#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Editor
{
public:
	bool runtimePause = false;

	void Init(GLFWwindow* window);
	void StartGUILayout();
	void CreateGUILayout(GLFWwindow* window, Camera& cam);
	void SaveGUILayout();
	void Delete();
};
