/*#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "model.h"

struct Transform 
{
	glm::vec3 position{ 0 };
	glm::vec3 rotation{ 0 };
	glm::vec3 scale{ 1 };

	glm::mat4 matrix{ 1.0f };

	// Update automatically the matrix when any property changes (IMPLEMENT THIS!!!)
	void UpdateMatrix() {
		glm::mat4 m = glm::mat4(1.0f);

		m = glm::translate(m, position);
		m = glm::rotate(m, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		m = glm::rotate(m, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		m = glm::rotate(m, glm::radians(rotation.z), glm::vec3(0, 0, 1));
		m = glm::scale(m, scale);

		matrix = m;
	}
};


class GameObject 
{
public:
		// INFO AND PROPERTIES
		std::string name;
		Transform transform;
		Model model;

		void SetActive(bool active);

		// METHODS

		void Destroy();
		void Instantiate(GameObject* prefab);
		void Render();

		// CONSTRUCTOR 
		explicit GameObject(const std::string& objName = "GameObject", const char* path, Shader& programShader, Camera& globalCamera);

		// TYPENAMES => COMPONENTS
		template<typename T>
		T* AddComponent();

		template<typename T>
		T* GetComponent();

		template<typename T>
		T* TryGetComponent();

private:

		bool isActive;
		Shader shader;
		Camera camera;
};*/