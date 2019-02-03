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
protected:
  Vec2 corners[4];
  Vec2 edge01, edge03;
  float edge01Min, edge01Max, edge03Min, edge03Max;
  Vec2 distSquared;
  float sumRadiiSquared;
  bool rotatedBoxReady;

public:
  CollisionType collisionType;
  CollisionResponse collisionResponse;
  CSprite sprite;
  RECT edge = {-1, -1, 1, 1};
  float radius;
  char* collisionId;

  bool equals(CCollidable cmp) { return (collisionId == cmp.collisionId); }

  const Vec2* getCorner(uint32 c) const
  {
    if (c >= 4) c = 0;
    return &corners[c];
  }

  bool collideCircle(CCollidable& c, Vec2& collisionVector)
  {
    distSquared = sprite.getCenter() - c.sprite.getCenter();
    distSquared.x = distSquared.x * distSquared.x;
    distSquared.y = distSquared.y * distSquared.y;

    sumRadiiSquared =
        (radius * sprite.getScale()) + (c.radius * c.sprite.getScale());
    sumRadiiSquared *= sumRadiiSquared;

    if (distSquared.x + distSquared.y <= sumRadiiSquared) {
      collisionVector = c.sprite.getCenter() - sprite.getCenter();
      return true;
    }
    return false;
  }

  bool collideBox(CCollidable& c, Vec2& collisionVector)
  {

    if ((sprite.getCenterX() + edge.right * sprite.getScale() <
         c.sprite.getCenterX() + c.edge.left * c.sprite.getScale()) ||
        (sprite.getCenterX() + edge.left * sprite.getScale() >
         c.sprite.getCenterX() + c.edge.right * c.sprite.getScale()) ||
        (sprite.getCenterY() + edge.bottom * sprite.getScale() <
         c.sprite.getCenterY() + c.edge.top * c.sprite.getScale()) ||
        (sprite.getCenterY() + edge.top * sprite.getScale() >
         c.sprite.getCenterY() + c.edge.bottom * c.sprite.getScale())) {

      return false;
    }

    collisionVector = c.sprite.getCenter() - sprite.getCenter();
    return true;
  }

  bool collideRotatedBox(CCollidable& c, Vec2& collisionVector)
  {
    computeRotatedBox();
    c.computeRotatedBox();
    if (projectionsOverlap(c) && c.projectionsOverlap(*this)) {
      collisionVector = c.sprite.getCenter() - sprite.getCenter();
      return true;
    }
    return false;
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
  bool collideRotatedBoxCircle(CCollidable& c, Vec2& collisionVector)
  {
    Logger::println("Checking for collision");
    float min01, min03, max01, max03, center01, center03;

    computeRotatedBox();

    center01 = Vec2NS::Vector2Dot(&edge01, &c.sprite.getCenter());
    min01 = center01 - c.radius * c.sprite.getScale();
    max01 = center01 + c.radius * c.sprite.getScale();
    if (min01 > edge01Max || max01 < edge01Min) return false;

    center03 = Vec2NS::Vector2Dot(&edge03, &c.sprite.getCenter());
    min03 = center03 - c.radius * c.sprite.getScale();
    max03 = center03 + c.radius * c.sprite.getScale();
    if (min03 > edge03Max || max03 < edge03Min) return false;

    if (center01 < edge01Min && center03 < edge03Min)
      return collideCornerCircle(corners[0], c, collisionVector);
    if (center01 > edge01Max && center03 < edge03Min)
      return collideCornerCircle(corners[1], c, collisionVector);
    if (center01 > edge01Max && center03 > edge03Max)
      return collideCornerCircle(corners[2], c, collisionVector);
    if (center01 < edge01Min && center03 > edge03Max)
      return collideCornerCircle(corners[3], c, collisionVector);

    collisionVector = c.sprite.getCenter() - sprite.getCenter();
    return true;
  }
  bool collideCornerCircle(Vec2 corner, CCollidable& c, Vec2& collisionVector)
  {
    distSquared = corner - c.sprite.getCenter();
    distSquared.x = distSquared.x * distSquared.x;
    distSquared.y = distSquared.y * distSquared.y;

    sumRadiiSquared = c.radius * c.sprite.getScale();
    sumRadiiSquared *= sumRadiiSquared;

    if (distSquared.x + distSquared.y <= sumRadiiSquared) {
      collisionVector = c.sprite.getCenter() - corner;
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

    const Vec2 center = sprite.getCenter();
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
        if (collisionComponents.at(i).equals(*collidable)) componentIndex = i;
      }
    }

    if (componentIndex == -1) {
      Logger::println("Added collision component");
      collisionComponents.push_back(*collidable);
    } else {
      for (int i = 0; i < collisionComponents.size(); i++) {
        if (i == componentIndex) continue;

        bool didCollide;
        Vec2 collisionVector = Vec2(0.0, 0.0);
        CCollidable collidable2 = collisionComponents.at(i);

        if (collidable->collisionType == CollisionType::CIRCLE &&
            collidable2.collisionType == CollisionType::CIRCLE)
          didCollide = collidable->collideCircle(collidable2, collisionVector);

        if (collidable->collisionType == CollisionType::BOX &&
            collidable2.collisionType == CollisionType::BOX)
          didCollide = collidable->collideBox(collidable2, collisionVector);

        if (collidable->collisionType != CollisionType::CIRCLE &&
            collidable2.collisionType != CollisionType::CIRCLE)
          didCollide =
              collidable->collideRotatedBox(collidable2, collisionVector);

        else if (collidable->collisionType == CollisionType::CIRCLE) {
          didCollide =
              collidable2.collideRotatedBoxCircle(*collidable, collisionVector);
          collisionVector *= -1;
        } else
          didCollide =
              collidable->collideRotatedBoxCircle(collidable2, collisionVector);

        if (didCollide) Logger::println("Collision detected");
      }
    }
  }
};