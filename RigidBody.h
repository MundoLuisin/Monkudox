#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include "PhysicsWorld.h"
#include "Collider.h"

struct Constraints
{
    bool FreezePositionX = false;
    bool FreezePositionY = false;
    bool FreezePositionZ = false;
    bool FreezeRotationX = false;
    bool FreezeRotationY = false;
    bool FreezeRotationZ = false;
};

class RigidBody
{
public:
    RigidBody(PhysicsWorld* world, const Collider& collider, const JPH::Vec3& position, bool isStatic);
    ~RigidBody();

    float mass = 1.0f;
    float linearDamping = 0.0f;
    float angularDamping = 0.0f;

    bool isKinematic = false;

    enum class Interpolate { None, Interpolate, Extrapolate };
    enum class CollisionDetection { Discrete, Continuous };

    Constraints constraints;
    LayerOverrides layerOverrides;

    JPH::BodyID GetBodyID() const { return mBodyID; }

private:
    PhysicsWorld* mWorld;
    JPH::BodyID mBodyID;
};
