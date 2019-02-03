#pragma once

#include "common.h"
#include "ecs.h"
#include "systems/physics.h"

enum CollisionResponse { BOUNCE, NONE };
enum CollisionType { BOX, CIRCLE, ORIENTED_BOX };
struct CCollidable : public Component<CCollidable> {
  CollisionResponse collisionResponse;
  CollisionType collisionType;

  RECT edge;
  Vec2 corners[4];
  Vec2 edge01, edge03;
  float edge01Min, edge01Max, edge03Min, edge03Max;
  float radius;
  Vec2 distSquared;
  float sumRadiiSquared;
  bool rotatedBoxReady;

public:
  bool collideCircle(CCollidable& c, Vec2& collisionVector) {}
  bool collideBox(CCollidable& c, Vec2& collisionVector) {}
  bool collideRotatedBox(CCollidable& c, Vec2& collisionVector) {}
  bool projectionsOverlap(CCollidable& c) {}
  bool collideRotatedBoxCircle(CCollidable& c, Vec2& collisionVector) {}
  bool collideCornerCircle(Vec2 corner, CCollidable& c, Vec2& collisionVector)
  {
  }
  void computeRotatedBox() {}
};

class SCollision : public System
{
  Array<CCollidable*> collisionComponents;

public:
  SCollision() : System()
  {
    System::addComponentType(CCollidable::id);
    System::addComponentType(CMotion::id);
  }
  virtual void updateComponents(float delta, BaseComponent** components)
  {
    CCollidable* collidable = (CCollidable*)components[0];
    CMotion* motion = (CMotion*)components[1];

    collisionComponents[collidable->id] =
        collidable; // Keep live collection of pointers to collidables

    for (int i = 0; i < collisionComponents.size(); i++) {
      if (i == collidable->id) continue;

      bool didCollide;
      Vec2 collisionVector = Vec2(0.0, 0.0);
      CCollidable* collidable2 = collisionComponents[i];

      if (collidable->collisionType == CollisionType::CIRCLE &&
          collidable2->collisionType == CollisionType::CIRCLE)
        didCollide = collidable->collideCircle(*collidable2, collisionVector);

      if (collidable->collisionType == CollisionType::BOX &&
          collidable2->collisionType == CollisionType::BOX)
        didCollide = collidable->collideBox(*collidable2, collisionVector);

      if (collidable->collisionType != CollisionType::CIRCLE &&
          collidable2->collisionType != CollisionType::CIRCLE)
        didCollide =
            collidable->collideRotatedBox(*collidable2, collisionVector);

      else if (collidable->collisionType == CollisionType::CIRCLE) {
        didCollide =
            collidable2->collideRotatedBoxCircle(*collidable, collisionVector);
        collisionVector *= -1;
      } else
        didCollide =
            collidable->collideRotatedBoxCircle(*collidable2, collisionVector);
    }
  }
};