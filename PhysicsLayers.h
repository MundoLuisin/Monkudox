#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

namespace PhysicsLayers
{
    static constexpr JPH::ObjectLayer DEFAULT = 0;
    static constexpr JPH::ObjectLayer TRANSPARENTFX = 1;
    static constexpr JPH::ObjectLayer IGNORERAYCAST = 2;
    static constexpr JPH::ObjectLayer WATER = 3;
    static constexpr JPH::ObjectLayer UI = 4;
    static constexpr JPH::ObjectLayer PLAYER = 5;
    static constexpr JPH::ObjectLayer NUM_LAYERS = 6;

    static constexpr JPH::BroadPhaseLayer BP_NON_MOVING(0);
    static constexpr JPH::BroadPhaseLayer BP_MOVING(1);      
    static constexpr JPH::BroadPhaseLayer NUM_BROAD_PHASE_LAYERS(2);
}

// Decide si dos capas de objetos pueden chocar
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::ObjectLayer inLayer2) const override {
        switch (inLayer1) {
        case PhysicsLayers::DEFAULT: return true; // Todo choca con default
        case PhysicsLayers::PLAYER:  return inLayer2 != PhysicsLayers::PLAYER; // Ejemplo: jugadores no chocan entre sí
        default: return true;
        }
    }
};

// Mapea capas de objetos a BroadPhase (estático o móvil)
class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
public:
    BPLayerInterfaceImpl() {
        mObjectToBroadPhase[PhysicsLayers::DEFAULT] = PhysicsLayers::BP_NON_MOVING;
        mObjectToBroadPhase[PhysicsLayers::PLAYER] = PhysicsLayers::BP_MOVING;
        mObjectToBroadPhase[PhysicsLayers::TRANSPARENTFX] = PhysicsLayers::BP_NON_MOVING;
        mObjectToBroadPhase[PhysicsLayers::IGNORERAYCAST] = PhysicsLayers::BP_NON_MOVING;
        mObjectToBroadPhase[PhysicsLayers::WATER] = PhysicsLayers::BP_NON_MOVING;
        mObjectToBroadPhase[PhysicsLayers::UI] = PhysicsLayers::BP_NON_MOVING;
    }

    virtual unsigned int GetNumBroadPhaseLayers() const override {
        return PhysicsLayers::NUM_BROAD_PHASE_LAYERS.GetValue();
    }

    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override {
        return mObjectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    // ESTE ES EL MÉTODO QUE TE FALTA
    virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override {
        switch ((JPH::BroadPhaseLayer::Type)inLayer) {
        case (JPH::BroadPhaseLayer::Type)PhysicsLayers::BP_NON_MOVING: return "NON_MOVING";
        case (JPH::BroadPhaseLayer::Type)PhysicsLayers::BP_MOVING:     return "MOVING";
        default: return "INVALID";
        }
    }
#endif

private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[PhysicsLayers::NUM_LAYERS];
};

class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override {
        if (inLayer2 == PhysicsLayers::BP_NON_MOVING) return inLayer1 != PhysicsLayers::DEFAULT;
        return true;
    }
};