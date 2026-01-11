#pragma once

#include <string>
#include "model.h"
#include "Transform.h"
#include "Component.h"

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
		template<typename T, typename... Args>
		T* AddComponent(Args&&... args);

		template<typename T>
		T* GetComponent();

		template<typename T>
		T* TryGetComponent() { return GetComponent<T>(); }
		*/
private:
	   //std::unordered_map<const std::type_info*, std::unique_ptr<Component>> components;

		Shader& shader;
		Camera& camera;
};