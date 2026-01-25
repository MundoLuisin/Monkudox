#include "Collider.h"

Collider::Collider(const JPH::Shape* shape) : mShape(shape) {
    if (mShape) {
        // Incrementamos la referencia para que Jolt no limpie el Shape antes de tiempo
        mShape->AddRef();
    }
}

Collider::~Collider() {
    if (mShape) {
        mShape->Release();
    }
}