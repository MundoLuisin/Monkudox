#include "RigidBody.h"
#include <Jolt/Physics/Body/BodyInterface.h>

RigidBody::RigidBody(PhysicsWorld* world, const Collider& collider, const JPH::Vec3& position, bool isStatic)
    : mWorld(world) {

    JPH::BodyInterface& body_interface = mWorld->GetSystem().GetBodyInterface();

    JPH::BodyCreationSettings settings(
        collider.GetShape(),
        position,
        JPH::Quat::sIdentity(),
        isStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic,
        isStatic ? PhysicsLayers::DEFAULT : PhysicsLayers::PLAYER
    );

    mBodyID = body_interface.CreateAndAddBody(settings, JPH::EActivation::Activate);
}

RigidBody::~RigidBody() {
    if (mWorld) {
        mWorld->GetSystem().GetBodyInterface().RemoveBody(mBodyID);
        mWorld->GetSystem().GetBodyInterface().DestroyBody(mBodyID);
    }
}