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
  SMotion() : System() { addComponentType(CMotion::id); }

  virtual void updateComponents(float delta, BaseComponent** components)
  {
    CMotion* motion = (CMotion*)components[0];
    motion->velocity = Vec2(10.0, 10.0);

    OutputDebugString("updated motion component\n");
  }
};