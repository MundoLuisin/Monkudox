#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include "PhysicsWorld.h"

class Collider
{
public:
    Collider(const JPH::Shape* shape);
    ~Collider();

    bool isTrigger = false;
    bool providesContacts = true;

    JPH::Vec3 center = JPH::Vec3::sZero();
    JPH::Vec3 size = JPH::Vec3::sZero();

    LayerOverrides layerOverrides;

    const JPH::Shape* GetShape() const { return mShape; }

private:
    const JPH::Shape* mShape;
};
