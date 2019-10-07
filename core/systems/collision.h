#pragma once

#include "common.h"
#include "components/collidable.h"
#include "ecs.h"
#include "math/vector2.h"
#include "systems/physics.h"
#include "systems/renderable.h"

class SCollision : public System
{
  Map<int, CCollidable*> collisionComponents; // Keep local cache of collision
                                              // components for random access in
                                              // collision detection
  Rect collisionBounds = {
      0, 0, GAME_WIDTH, GAME_HEIGHT}; // Bounds that desribe the simulation area

public:
  SCollision() : System()
  {
    System::addComponentType(CSprite::id);
    System::addComponentType(CMotion::id);
    System::addComponentType(CCollidable::id);
  }

  void setBounds(Rect _collisionBounds) { collisionBounds = _collisionBounds; }

  virtual void updateComponents(float delta, BaseComponent** components)
  {
    CSprite* sprite = (CSprite*)components[0];
    CMotion* motion = (CMotion*)components[1];
    CCollidable* collidable = (CCollidable*)components[2];

    // Inform collider about sprite
    collidable->radius = sprite->getWidth() / 2;
    collidable->angle = sprite->getAngle();
    collidable->center = *(sprite->getCenter());
    collidable->scale = sprite->getScale();
    collidable->edge = {
        -(sprite->spriteData.width / 2), -(sprite->spriteData.height / 2),
        (sprite->spriteData.width / 2), (sprite->spriteData.height / 2)};
    // Inform collider about motion
    collidable->velocity = motion->velocity;

    // Update collision map
    if (collidable->collisionId == -1) {
      collidable->collisionId = collisionComponents.size();
    }

    collisionComponents[collidable->collisionId] = collidable;

    // Collision Detection
    bool didCollide = false;
    for (int i = 0; i < collisionComponents.size() - 1; i++) {

      if (i == collidable->collisionId) continue;

      Vec2 collisionVector = Vec2(0.0, 0.0);
      CCollidable collidable2 = *(collisionComponents[i]);

      if (collidable->collisionType == CollisionType::CIRCLE &&
          collidable2.collisionType == CollisionType::CIRCLE) {
        // didCollide = collidable->collideCircle(collidable2, collisionVector);
        didCollide =
            isCollidingCircle(*collidable, collidable2, collisionVector);

      } else if (collidable->collisionType == CollisionType::BOX &&
                 collidable2.collisionType == CollisionType::BOX) {
        didCollide = isCollidingBox(*collidable, collidable2, collisionVector);

      } else if (collidable->collisionType != CollisionType::CIRCLE &&
                 collidable2.collisionType != CollisionType::CIRCLE) {
        didCollide =
            collidable->collideRotatedBox(collidable2, collisionVector);

      } else if (collidable->collisionType == CollisionType::CIRCLE) {
        didCollide =
            collidable2.collideRotatedBoxCircle(*collidable, collisionVector);

      } else {
        didCollide =
            collidable->collideRotatedBoxCircle(collidable2, collisionVector);
      }

      if (didCollide) {
        // FIXME: Design flaw of the current implementation, we can't signal to
        // multiple collidables... probably can be fixed by message queue
        // collisionComponents[i]->colliding = true;
        motion->velocity =
            calculateResultVector(*collidable, collidable2, collisionVector);
      }
    }

    /////////////////////////////////
    // Edge Detection (Bounds)//
    /////////////////////////////////
    bool didCollideEdge = false;
    if (sprite->getX() >= collisionBounds.right - sprite->getWidth()) {
      didCollideEdge = true;
      motion->velocity.x = -2 * collidable->velocity.x;
      sprite->spriteData.x = collisionBounds.right - sprite->getWidth();
    } else if (sprite->getX() <= collisionBounds.left) {
      didCollideEdge = true;
      motion->velocity.x = -2 * collidable->velocity.x;
      sprite->spriteData.x = collisionBounds.left;
    }
    if (sprite->getY() >= collisionBounds.bottom - sprite->getHeight()) {
      didCollideEdge = true;
      motion->velocity.y = -2 * collidable->velocity.y;
      sprite->spriteData.y = collisionBounds.bottom - sprite->getHeight();
    } else if (sprite->getY() <= collisionBounds.top) {
      didCollideEdge = true;
      motion->velocity.y = -2 * collidable->velocity.y;
      sprite->spriteData.y = collisionBounds.top;
    }

    collidable->colliding = didCollideEdge || didCollide;
    motion->colliding =
        didCollideEdge ||
        didCollide; // Signal to motion system to apply simulated force
  }

private:
  Vec2 calculateResultVector(CCollidable c1, CCollidable c2,
                             Vec2 collisionVector)
  {
    Vec2 unitVector = collisionVector;
    Vec2NS::Vector2Normalize(&unitVector);
    Vec2 velocityDifference = c2.velocity - c1.velocity;
    float magnitude = Vec2NS::Vector2Dot(&unitVector, &velocityDifference);
    float massRatio = c1.mass / c2.mass;
    return ((massRatio * magnitude) * unitVector);
  }

  bool isCollidingBox(CCollidable c1, CCollidable c2, Vec2& collisionVector)
  {
    if (((c1.center.x + c1.edge.right) * c1.scale <
         (c2.center.x + c2.edge.left) * c2.scale) ||
        ((c1.center.x + c1.edge.left) * c1.scale >
         (c2.center.x + c2.edge.right) * c2.scale) ||
        (c1.center.y + c1.edge.bottom) * c1.scale <
            (c2.center.y + c2.edge.top) * c2.scale ||
        (c1.center.y + c1.edge.top) * c1.scale >
            (c2.center.y + c2.edge.bottom) * c2.scale) {
      return false;
    }
    collisionVector = c2.center - c1.center;
    return true;
  }

  bool isCollidingCircle(CCollidable c1, CCollidable c2, Vec2& collisionVector)
  {
    Vec2 distSquared = c1.center - c2.center;
    distSquared.x *= distSquared.x;
    distSquared.y *= distSquared.y;

    float sumRadiiSquared = c1.radius + c2.radius;
    sumRadiiSquared *= sumRadiiSquared;

    if (distSquared.x + distSquared.y <= sumRadiiSquared) {
      collisionVector = c2.center - c1.center;
      return true;
    }
    return false;
  }

  bool collideRotatedBox(CCollidable c1, CCollidable c2, Vec2& collisionVector)
  {
    c1.computeRotatedBox();
    c2.computeRotatedBox();
    if (c1.projectionsOverlap(c2) && c2.projectionsOverlap(c1)) {
      collisionVector = c2.center - c1.center;
      return true;
    }
    return false;
  }
};