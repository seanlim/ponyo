#pragma once

#include "common.h"
#include "ecs.h"
#include "logger.h"
#include "systems/physics.h"
#include "systems/renderable.h"
#include <set>

enum CollisionResponse { BOUNCE, NONE };
enum CollisionType { BOX, CIRCLE, ORIENTED_BOX };
struct CCollidable : public Component<CCollidable> {
public:
  Vec2 corners[4];
  Vec2 edge01, edge03;
  float edge01Min, edge01Max, edge03Min, edge03Max;
  Vec2 distSquared;
  float sumRadiiSquared;
  bool rotatedBoxReady = false;
  CollisionType collisionType;
  CollisionResponse collisionResponse;
  CSprite sprite;
  RECT edge = {-1, -1, 1, 1};
  float radius;
  uint32 collisionId;

  bool equals(CCollidable cmp) { return (collisionId == cmp.collisionId); }

  const Vec2* getCorner(uint32 c) const
  {
    if (c >= 4) c = 0;
    return &corners[c];
  }

  bool projectionsOverlap(CCollidable& c)
  {
    float projection, min01, max01, min03, max03;

    projection = Vec2NS::Vector2Dot(&edge01, c.getCorner(0));
    min01 = projection;
    max01 = projection;
    for (int i = 1; i < 4; i++) {
      projection = Vec2NS::Vector2Dot(&edge01, c.getCorner(i));
      if (projection < min01)
        min01 = projection;
      else if (projection > max01)
        max01 = projection;
    }
    if (min01 > edge01Max || max01 < edge01Min) return false;

    projection = Vec2NS::Vector2Dot(&edge03, c.getCorner(0));
    min03 = projection;
    max03 = projection;
    for (int i = 1; i < 4; i++) {
      projection = Vec2NS::Vector2Dot(&edge03, c.getCorner(i));
      if (projection < min03)
        min03 = projection;
      else if (projection > max03)
        max03 = projection;
    }
    if (min03 > edge03Max || max03 < edge03Min) return false;

    return true;
  }
  bool collideCornerCircle(Vec2 corner, CCollidable& c, Vec2& collisionVector)
  {
    distSquared = corner - *c.sprite.getCenter();
    distSquared.x = distSquared.x * distSquared.x;
    distSquared.y = distSquared.y * distSquared.y;

    sumRadiiSquared = c.radius * c.sprite.getScale();
    sumRadiiSquared *= sumRadiiSquared;

    if (distSquared.x + distSquared.y <= sumRadiiSquared) {
      collisionVector = *c.sprite.getCenter() - corner;
      return true;
    }
    return false;
  }
  void computeRotatedBox()
  {
    if (rotatedBoxReady) return;
    float projection;

    Vec2 rotatedX(cos(sprite.spriteData.angle), sin(sprite.spriteData.angle));
    Vec2 rotatedY(-sin(sprite.spriteData.angle), cos(sprite.spriteData.angle));

    const Vec2 center = *sprite.getCenter();
    corners[0] = center + rotatedX * ((float)edge.left * sprite.getScale()) +
                 rotatedY * ((float)edge.top * sprite.getScale());
    corners[1] = center + rotatedX * ((float)edge.right * sprite.getScale()) +
                 rotatedY * ((float)edge.top * sprite.getScale());
    corners[2] = center + rotatedX * ((float)edge.right * sprite.getScale()) +
                 rotatedY * ((float)edge.bottom * sprite.getScale());
    corners[3] = center + rotatedX * ((float)edge.left * sprite.getScale()) +
                 rotatedY * ((float)edge.bottom * sprite.getScale());

    edge01 = Vec2(corners[1].x - corners[0].x, corners[1].y - corners[0].y);
    Vec2NS::Vector2Normalize(&edge01);
    edge03 = Vec2(corners[3].x - corners[0].x, corners[3].y - corners[0].y);
    Vec2NS::Vector2Normalize(&edge03);

    projection = Vec2NS::Vector2Dot(&edge01, &corners[0]);
    edge01Min = projection;
    edge01Max = projection;

    projection = Vec2NS::Vector2Dot(&edge01, &corners[1]);
    if (projection < edge01Min)
      edge01Min = projection;
    else if (projection > edge01Max)
      edge01Max = projection;

    projection = Vec2NS::Vector2Dot(&edge03, &corners[0]);
    edge03Min = projection;
    edge03Max = projection;
    projection = Vec2NS::Vector2Dot(&edge03, &corners[3]);
    if (projection < edge03Min)
      edge03Min = projection;
    else if (projection > edge03Max)
      edge03Max = projection;

    rotatedBoxReady = true;
  }
};

class SCollision : public System
{

  Array<CCollidable> collisionComponents;

public:
  SCollision() : System()
  {
    System::addComponentType(CSprite::id);
    System::addComponentType(CMotion::id);
    System::addComponentType(CCollidable::id);
  }
  virtual void updateComponents(float delta, BaseComponent** components)
  {
    CSprite* sprite = (CSprite*)components[0];
    CMotion* motion = (CMotion*)components[1];
    CCollidable* collidable = (CCollidable*)components[2];

    // Inform collision component about sprite
    collidable->sprite = *sprite;

    int componentIndex = -1;

    if (collisionComponents.size() > 0) {
      for (int i = 0; i < collisionComponents.size(); i++) {
        if (collisionComponents.at(i).equals(*collidable)) {
          componentIndex = i;
          collisionComponents[i] = *collidable;
        }
      }
    }

    if (componentIndex == -1) {
      collidable->collisionId = collisionComponents.size() + 1;
      collisionComponents.push_back(*collidable);
    } else {
      for (int i = 0; i < collisionComponents.size(); i++) {
        if (i == componentIndex) continue;

        bool didCollide;
        Vec2 collisionVector = Vec2(0.0, 0.0);
        CCollidable collidable2 = collisionComponents.at(i);

        if (collidable->collisionType == CollisionType::CIRCLE &&
            collidable2.collisionType == CollisionType::CIRCLE) {
          didCollide = collideCircle(*collidable, collidable2, collisionVector);
        }

        else if (collidable->collisionType == CollisionType::BOX &&
                 collidable2.collisionType == CollisionType::BOX) {
          didCollide = collideBox(*collidable, collidable2, collisionVector);
        }

        else if (collidable->collisionType != CollisionType::CIRCLE &&
                 collidable2.collisionType != CollisionType::CIRCLE)
          didCollide =
              collideRotatedBox(*collidable, collidable2, collisionVector);

        else if (collidable->collisionType == CollisionType::CIRCLE) {
          didCollide = collideRotatedBoxCircle(collidable2, *collidable,
                                               collisionVector);
          collisionVector *= -1;
        } else {
          didCollide = collideRotatedBoxCircle(*collidable, collidable2,
                                               collisionVector);
        }

        if (didCollide) Logger::println("Collision Detected!");
      }
    }
  }

protected:
  bool collideCircle(CCollidable& c1, CCollidable& c2, Vec2& collisionVector)
  {
    c1.distSquared = *c1.sprite.getCenter() - *c2.sprite.getCenter();
    c1.distSquared.x = c1.distSquared.x * c1.distSquared.x;
    c1.distSquared.y = c1.distSquared.y * c1.distSquared.y;

    c1.sumRadiiSquared =
        (c1.radius * c1.sprite.getScale()) + (c2.radius * c2.sprite.getScale());
    c1.sumRadiiSquared *= c1.sumRadiiSquared;

    if (c1.distSquared.x + c1.distSquared.y <= c1.sumRadiiSquared) {
      collisionVector = *c2.sprite.getCenter() - *c1.sprite.getCenter();
      return true;
    }
    return false;
  }

  bool collideBox(CCollidable& c1, CCollidable& c2, Vec2& collisionVector)
  {
    if ((c1.sprite.getCenterX() + c1.edge.right * c1.sprite.getScale() <
         c2.sprite.getCenterX() + c2.edge.left * c2.sprite.getScale()) ||
        (c1.sprite.getCenterX() + c1.edge.left * c1.sprite.getScale() >
         c2.sprite.getCenterX() + c2.edge.right * c2.sprite.getScale()) ||
        (c1.sprite.getCenterY() + c1.edge.bottom * c1.sprite.getScale() <
         c2.sprite.getCenterY() + c2.edge.top * c2.sprite.getScale()) ||
        (c1.sprite.getCenterY() + c1.edge.top * c1.sprite.getScale() >
         c2.sprite.getCenterY() + c2.edge.bottom * c2.sprite.getScale())) {

      return false;
    }

    collisionVector = *c2.sprite.getCenter() - *c1.sprite.getCenter();
    return true;
  }

  bool collideRotatedBox(CCollidable& c1, CCollidable& c2,
                         Vec2& collisionVector)
  {
    c1.computeRotatedBox(), c2.computeRotatedBox();
    if (c1.projectionsOverlap(c2) && c2.projectionsOverlap(c1)) {
      collisionVector = *c2.sprite.getCenter() - *c1.sprite.getCenter();
      return true;
    }
    return false;
  }

  bool collideRotatedBoxCircle(CCollidable& c1, CCollidable& c2,
                               Vec2& collisionVector)
  {
    float min01, min03, max01, max03, center01, center03;

    c1.computeRotatedBox();

    center01 = Vec2NS::Vector2Dot(&c1.edge01, c2.sprite.getCenter());
    min01 = center01 - c1.radius * c2.sprite.getScale();
    max01 = center01 + c1.radius * c2.sprite.getScale();
    if (min01 > c1.edge01Max || max01 < c1.edge01Min) return false;

    center03 = Vec2NS::Vector2Dot(&c1.edge03, c2.sprite.getCenter());
    min03 = center03 - c1.radius * c2.sprite.getScale();
    max03 = center03 + c1.radius * c2.sprite.getScale();
    if (min03 > c1.edge03Max || max03 < c1.edge03Min) return false;

    if (center01 < c1.edge01Min && center03 < c1.edge03Min)
      return c1.collideCornerCircle(c1.corners[0], c2, collisionVector);
    if (center01 > c1.edge01Max && center03 < c1.edge03Min)
      return c1.collideCornerCircle(c1.corners[1], c2, collisionVector);
    if (center01 > c1.edge01Max && center03 > c1.edge03Max)
      return c1.collideCornerCircle(c1.corners[2], c1, collisionVector);
    if (center01 < c1.edge01Min && center03 > c1.edge03Max)
      return c1.collideCornerCircle(c1.corners[3], c2, collisionVector);

    collisionVector = *c2.sprite.getCenter() - *c1.sprite.getCenter();
    return true;
  }
};