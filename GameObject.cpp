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

/*
// ── AddComponent ──────────────────────────────────────────────
template<typename T, typename... Args>
T* GameObject::AddComponent(Args&&... args)
{
    static_assert(std::is_base_of_v<Component, T>,
        "T debe heredar de Component!");

    // Evitamos crear dos veces el mismo tipo
    if (GetComponent<T>() != nullptr)
    {
        std::cout << "Warning: ya existe componente " << typeid(T).name() << "\n";
        return GetComponent<T>();
    }

    // Creamos el componente
    auto component = std::make_unique<T>(std::forward<Args>(args)...);

    // Le asignamos el owner (muy importante)
    component->gameObject = this;

    // Guardamos puntero crudo para devolver
    T* rawPtr = component.get();

    // Movemos la unique_ptr al mapa, usando typeid como clave
    components[&typeid(T)] = std::move(component);

    return rawPtr;
}

// ── GetComponent ──────────────────────────────────────────────
template<typename T>
T* GameObject::GetComponent()
{
    static_assert(std::is_base_of_v<Component, T>,
        "T debe heredar de Component!");

    auto it = components.find(&typeid(T));
    if (it != components.end())
    {
        // Es seguro hacer static_cast porque comprobamos con static_assert
        return static_cast<T*>(it->second.get());
    }

    return nullptr;
}
*/