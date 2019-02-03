#pragma once

#include "common.h"
#include "ecs.h"
#include "graphics.h"
#include "systems/renderable.h"

struct CMotion : public Component<CMotion> {
  Vec2 velocity = Vec2(0.0, 0.0);
  Vec2 acceleration = Vec2(0.0, 0.0);
  Vec2 gravity = Vec2(0.0, 9.81);

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
    motion->velocity += motion->acceleration * delta;

    // Friction?
    // motion->acceleration = Vec2(0.0, 0.0);

    // Move sprite
    sprite->spriteData.x += motion->velocity.x * delta;
    sprite->spriteData.y += motion->velocity.y * delta;
  }
};