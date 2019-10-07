#pragma once

#include "common.h"
#include "ecs.h"
#include "math/vector2.h"
#include "systems/physics.h"
#include "systems/renderable.h"

struct CCollidable : public Component<CCollidable> {
public:
  Vec2 corners[4];
  Vec2 edge01, edge03;
  float edge01Min, edge01Max, edge03Min, edge03Max;
  CollisionType collisionType;
  CollisionResponse collisionResponse;
  Rect edge = {-1, -1, 1, 1};
  uint32 collisionId = -1;
  float radius;
  float angle;
  bool colliding;
  Vec2 center;
  float scale;
  Vec2 velocity;
  float mass;

  bool equals(CCollidable* cmp) { return (collisionId == cmp->collisionId); }

  //////////////
  // Collision//
  //////////////

  //   bool collideCircle(CCollidable& c2, Vec2& collisionVector)
  //   {
  //     Vec2 distSquared = Vec2((center.x - c2.center.x), (center.y -
  //     c2.center.y)); distSquared.x *= distSquared.x; distSquared.y *=
  //     distSquared.y;

  //     sumRadiiSquared = radius + c2.radius;
  //     sumRadiiSquared *= sumRadiiSquared;

  //     if (distSquared.x + distSquared.y <= sumRadiiSquared) {
  //       collisionVector = center - c2.center;
  //       return true;
  //     }
  //     return false;
  //   }

  bool collideRotatedBox(CCollidable& c2, Vec2& collisionVector)
  {
    computeRotatedBox(), c2.computeRotatedBox();
    if (projectionsOverlap(c2) && c2.projectionsOverlap(*this)) {
      collisionVector = c2.center - center;
      return true;
    }
    return false;
  }

  bool collideRotatedBoxCircle(CCollidable& c2, Vec2& collisionVector)
  {
    float min01, min03, max01, max03, center01, center03;

    computeRotatedBox();

    center01 = Vec2NS::Vector2Dot(&edge01, &c2.center);
    min01 = center01 - c2.radius * c2.scale;
    max01 = center01 + c2.radius * c2.scale;
    if (min01 > edge01Max || max01 < edge01Min) return false;

    center03 = Vec2NS::Vector2Dot(&edge03, &c2.center);
    min03 = center03 - c2.radius * c2.scale;
    max03 = center03 + c2.radius * c2.scale;
    if (min03 > edge03Max || max03 < edge03Min) return false;

    if (center01 < edge01Min && center03 < edge03Min)
      return collideCornerCircle(corners[0], c2, collisionVector);
    if (center01 > edge01Max && center03 < edge03Min)
      return collideCornerCircle(corners[1], c2, collisionVector);
    if (center01 > edge01Max && center03 > edge03Max)
      return collideCornerCircle(corners[2], c2, collisionVector);
    if (center01 < edge01Min && center03 > edge03Max)
      return collideCornerCircle(corners[3], c2, collisionVector);

    collisionVector = c2.center - center;
    return true;
  }

  //////////////////////
  // Collision helpers//
  //////////////////////
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
    Vec2 distSquared = corner - c.center;
    distSquared.x = distSquared.x * distSquared.x;
    distSquared.y = distSquared.y * distSquared.y;

    float sumRadiiSquared = c.radius * c.scale;
    sumRadiiSquared *= sumRadiiSquared;

    if (distSquared.x + distSquared.y <= sumRadiiSquared) {
      collisionVector = c.center - corner;
      return true;
    }
    return false;
  }
  void computeRotatedBox()
  {
    float projection;

    Vec2 rotatedX(cos(angle), sin(angle));
    Vec2 rotatedY(-sin(angle), cos(angle));

    corners[0] = center + rotatedX * ((float)edge.left * scale) +
                 rotatedY * ((float)edge.top * scale);
    corners[1] = center + rotatedX * ((float)edge.right * scale) +
                 rotatedY * ((float)edge.top * scale);
    corners[2] = center + rotatedX * ((float)edge.right * scale) +
                 rotatedY * ((float)edge.bottom * scale);
    corners[3] = center + rotatedX * ((float)edge.left * scale) +
                 rotatedY * ((float)edge.bottom * scale);

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
  }
};