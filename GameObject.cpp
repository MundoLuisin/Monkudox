#include "GameObject.h"

void GameObject::SetActive(bool active)
{
	isActive = active;
}

GameObject::GameObject(const std::string& objName, const char* path, Shader& programShader, Camera& globalCamera)
    : name(objName),
    shader(programShader),
    camera(globalCamera),
    model(path)
{
    isActive = true;
}

void GameObject::Render()
{
    if(isActive)
    { 
		model.Draw(shader, camera, transform.matrix);
    }
}

template<typename T, typename... Args>
T* GameObject::AddComponent(Args&&... args)
{
    static_assert(std::is_base_of_v<Component, T>,
        "¡Nooo! T tiene que ser una waifu que herede de Component mamá (≧﹏≦)");

    // Creamos la waifu con los argumentos que le pasaste ♡
    auto component = std::make_unique<T>(std::forward<Args>(args)...);

    // Le decimos quién es su senpai (el GameObject)
    component->gameObject = this;

    // Guardamos la waifu en la lista de novias del GameObject
    T* ptr = component.get();           // puntero crudo para devolver
    components.push_back(std::move(component));  // transferimos propiedad

    return ptr;
}

template<typename T>
T* GameObject::GetComponent()
{
    static_assert(std::is_base_of_v<Component, T>,
        "Onii-chan... esa no es una Component-chan válida (ಥ﹏ಥ)");

    for (auto& comp : components)
    {
        if (T* derived = dynamic_cast<T*>(comp.get()))
        {
            return derived;   // ¡Encontré a la waifu que buscabas! ♡
        }
    }
    return nullptr;
}

template<typename T>
T* GameObject::TryGetComponent()
{ 
    return GetComponent<T>();
}