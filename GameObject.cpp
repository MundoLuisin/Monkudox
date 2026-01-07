/*#include "GameObject.h"

void GameObject::SetActive(bool active)
{
	isActive = active;
}

GameObject::GameObject(const std::string& objName, const char* path ,Shader& programShader, Camera& globalCamera) : name(objName), shader(programShader), camera(globalCamera)
{
	isActive = true;
	Model newModel(path);
	model = newModel;
}

void GameObject::Render()
{
    if(isActive)
    { 
		model.Draw(shader, camera, transform.matrix);
    }
}

template<typename T>
T* GameObject::AddComponent() {
    T* newComponent = new T();
    newComponent->gameObject = this;        
    newComponent->transform = this->transform;
    components[typeid(T)] = newComponent;

    return newComponent;
}

template<typename T>
T* GameObject::GetComponent() {
    auto it = components.find(typeid(T));
    if (it != components.end()) {
        return static_cast<T*>(it->second);
    }
    return nullptr;
}

template<typename T>
T* GameObject::GetComponent() {
    try
    {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            return static_cast<T*>(it->second);
        }
        return nullptr;
    }
    catch (const std::exception& error)
    {
		std::cout << "GetComponent Error: " << error.what() << std::endl;
    }
}*/