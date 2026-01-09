#pragma once

#include <string>
#include "model.h"
#include "Transform.h"

class GameObject 
{
public:
		// INFO AND PROPERTIES
		std::string name;
		Transform transform;
		Model model;
		bool isActive;

		void SetActive(bool active);

		// METHODS

		void Destroy();
		void Instantiate(GameObject* prefab);
		void Render();

		// CONSTRUCTOR 
		explicit GameObject(const std::string& objName, const char* path, Shader& programShader, Camera& globalCamera);

		// TYPENAMES => COMPONENTS
		/*
		template<typename T>
		T* AddComponent();

		template<typename T>
		T* GetComponent();

		template<typename T>
		T* TryGetComponent();*/

private:

		Shader& shader;
		Camera& camera;
};