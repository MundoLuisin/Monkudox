#include "PhysicsWorld.h"

PhysicsWorld::PhysicsWorld() 
{
    // ✅ AHORA creamos los filtros (DESPUÉS de RegisterTypes)
    bp_interface = new BPLayerInterfaceImpl();
    obj_vs_obj_filter = new ObjectLayerPairFilterImpl();
    obj_vs_bp_filter = new ObjectVsBroadPhaseLayerFilterImpl();

    mTempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024); // 10MB
    mJobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, 1);

    mPhysicsSystem.Init(1024, 0, 1024, 10, *bp_interface, *obj_vs_bp_filter, *obj_vs_obj_filter);
}

void PhysicsWorld::Step(float deltaTime) {
    // 1 paso de física, 1 sub-paso
    mPhysicsSystem.Update(deltaTime, 1, mTempAllocator, mJobSystem);
}

PhysicsWorld::~PhysicsWorld() {
    delete mTempAllocator;
    delete mJobSystem;

    // ⚠️ IMPORTANTE: Limpiar la factory al final
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}