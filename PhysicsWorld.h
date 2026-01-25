#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/RegisterTypes.h>
#include "PhysicsLayers.h"


struct LayerOverrides
{
    unsigned int LayerOverridePriority = 0;
    std::vector<JPH::ObjectLayer> IncludeLayers;
    std::vector<JPH::ObjectLayer> ExcludeLayers;
};

class PhysicsWorld
{
public:
    PhysicsWorld();
    ~PhysicsWorld();

    void Step(float deltaTime);

    JPH::PhysicsSystem& GetSystem() { return mPhysicsSystem; }

    void SetGravity(const JPH::Vec3& gravity) {
        mPhysicsSystem.SetGravity(gravity);
    }

private:
    JPH::PhysicsSystem mPhysicsSystem;
    JPH::TempAllocatorImpl* mTempAllocator;
    JPH::JobSystemThreadPool* mJobSystem;

    BPLayerInterfaceImpl* bp_interface;
    ObjectLayerPairFilterImpl* obj_vs_obj_filter;
    ObjectVsBroadPhaseLayerFilterImpl* obj_vs_bp_filter;
};
