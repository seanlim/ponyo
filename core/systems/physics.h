#pragma once

#include "common.h"
#include "ecs.h"
#include "math/lerp.h"
#include "systems/renderable.h"

struct CMotion : public Component<CMotion> {
  Vec2 velocity = Vec2(0.0, 0.0);
  Vec2 acceleration = Vec2(0.0, 0.0);
  Vec2 gravity = Vec2(0.0, 0.0);
  float friction = 0.3;
  float frictionSmoothness = 0.7;
  bool colliding = false;
  Vec2 collidedDelta; // Delta after physics simulation

  void setGravity(float acc) { gravity.y = acc; }
};

class SPhysics : public System
{
public:
  SPhysics() : System()
  {
    System::addComponentType(CMotion::id);
    System::addComponentType(CSprite::id);
  }

  virtual void updateComponents(float delta, BaseComponent** components)
  {
    CMotion* motion = (CMotion*)components[0];
    CSprite* sprite = (CSprite*)components[1];

    // Physics execution
    motion->acceleration += motion->gravity;

    // Apply forces
    motion->velocity += motion->acceleration * (delta * 100);

    // Handle collision
    if (motion->colliding) {
      motion->velocity += motion->collidedDelta;
    }

    // Move sprite
    sprite->spriteData.x += motion->velocity.x * delta;
    sprite->spriteData.y += motion->velocity.y * delta;

    if (motion->acceleration.x == 0)
      motion->velocity.x = lerp(
          motion->velocity.x, motion->velocity.x * (motion->frictionSmoothness),
          motion->friction);
    if (motion->acceleration.y == 0)
      motion->velocity.y = lerp(
          motion->velocity.y, motion->velocity.y * (motion->frictionSmoothness),
          motion->friction);
    motion->acceleration = Vec2(0.0, 0.0);
  }
};