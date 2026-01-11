#pragma once

#include "GameObject.h"

namespace MonkudoxEngine
{
    class MonoBehaviour {
    public:
        virtual ~MonoBehaviour() = default;
        virtual void Awake() {}
        virtual void Start() {}
        virtual void Update() {}
    };

    class GameController : public MonoBehaviour {
    public:
        std::vector<GameObject> allGameObjects;

        void Awake() override;
        void Start() override;
        void Update() override;
    };
}
