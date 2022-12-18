#include "Collider.h"


//-----------------------------------------------------------
uint32_t Collider::GetCollisionAttribute()
{
    return uint32_t(collisionAttribute_);
}

void Collider::SetCollisionAttribute(const uint32_t& bit)
{
    collisionAttribute_ = bit;
}

uint32_t Collider::GetCollisionMask()
{
    return uint32_t(collisionMask_);
}

void Collider::SetCollisionMask(const uint32_t& bit)
{
    collisionMask_ = bit;
}
