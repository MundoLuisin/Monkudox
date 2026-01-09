#pragma once
#include <glm/glm.hpp>

struct RigidBody
{
    glm::vec3 position;
    glm::vec3 velocity;
    bool grounded;
};
