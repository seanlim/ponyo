#pragma once

#include "common.h"
#include "ecs.h"
#include "graphics.h"

struct CMotion : public Component<CMotion> {
  Vec2 velocity = Vec2(0.0, 0.0);
  Vec2 acceleration = Vec2(0.0, 0.0);
};

class SMotion : public System
{
public:
  SMotion() : System() { System::addComponentType(CMotion::id); }

  virtual void updateComponents(float delta, BaseComponent** components)
  {
    CMotion* motion = (CMotion*)components[0];
    // Add gravity
    motion->velocity.y += 10;
  }
};