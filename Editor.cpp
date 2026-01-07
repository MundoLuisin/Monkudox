#include "Editor.h"

void Editor::Init(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = "../imgui.ini";
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void Editor::StartGUILayout()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Editor::CreateGUILayout(GLFWwindow* window)
{
	ImGui::Begin("Console");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("Inspector");
	ImGui::End();

	ImGui::Begin("Scene");
	if (ImGui::Button(runtimePause ? "Play" : "Pause")) {
		runtimePause = !runtimePause;
	}
	ImGui::End();

	ImGui::Begin("Hierarchy");
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Editor::SaveGUILayout()
{
	ImGui::SaveIniSettingsToDisk(ImGui::GetIO().IniFilename);
}

void Editor::Delete()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}