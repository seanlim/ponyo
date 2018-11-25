#pragma once
#define WIN32_LEAN_AND_MEAN

#include "image.h"
#include "input.h"
#include "game.h"

namespace entityNS
{
enum COLLISION_TYPE
{
  NONE,
  CIRCLE,
  BOX,
  ORIENTED_BOX
};
const float GRAVITY = 6.67428e-11f;
} // namespace entityNS

class Entity : public Image
{
protected:
  Input *input;

  VECTOR2 center;
  float radius;
  VECTOR2 distSquared;
  float sumRadiiSquared;
  RECT edge;

  VECTOR2 corners[4];
  VECTOR2 edge01, edge03;
  float edge01Min, edge01Max, edge03Min, edge03Max;

  VECTOR2 velocity;
  VECTOR2 deltaV;

  float mass;
  float health;
  float rr;
  float force;
  float gravity;

  HRESULT hr;

  bool active;          // Indicate ready for collision
  bool rotatedBoxReady; // Indicate when oriented box is ready

  // Collision
  entityNS::COLLISION_TYPE collisionType;

  virtual bool collideCircle(Entity &ent, VECTOR2 &collisionVector);
  virtual bool collideBox(Entity &ent, VECTOR2 &collisionVector);
  virtual bool collideRotatedBox(Entity &ent, VECTOR2 &collisionVector);
  virtual bool collideRotatedBoxCircle(Entity &ent, VECTOR2 &collisionVector);

  // Collision helpers
  void computeRotatedBox();
  bool projectionsOverlap(Entity &ent);
  bool collideCornerCircle(VECTOR2 corner, Entity &ent, VECTOR2 &collisionVector);

public:
  Entity();
  virtual ~Entity() {}

  // Getters
  virtual const VECTOR2 *getCenter()
  {
    return &VECTOR2(this->getCenterX(), this->getCenterY());
  }
  virtual const RECT &getEdge() const { return edge; }
  virtual const VECTOR2 *getCorner(UINT c) const
  {
    if (c >= 4)
      c = 0;
    return &this->corners[c];
  }
  virtual float getRadius() const { return this->radius; }
  virtual const VECTOR2 getVelocity() const { return this->velocity; }
  virtual bool getActive() const { return this->active; }
  virtual float getMass() const { return this->mass; }
  virtual float getGravity() const { return this->gravity; }
  virtual float getHealth() const { return this->health; }
  virtual entityNS::COLLISION_TYPE getCollisionType() const { return this->collisionType; }

  // Setters
  virtual void setVelocity(VECTOR2 v) { this->velocity = v; }
  virtual void setDeltaV(VECTOR2 dv) { this->deltaV = dv; }
  virtual void setActive(bool a) { this->active = a; }
  virtual void setHealth(float h) { this->health = h; }
  virtual void setMass(float m) { this->mass = m; }
  virtual void setGravity(float g) { this->gravity = g; }
  virtual void setCollisionRadius(float r) { this->radius = r; }

  virtual void update(float frameTime);
  virtual bool initialise(Game *game, int width, int height, int nCols, TextureManager *textureManager);
  virtual void activate();
  virtual void ai(float frameTime, Entity &ent);
  virtual bool outsideRect(RECT rect);
  virtual bool collidesWith(Entity &ent, VECTOR2 &collisionVector);
  virtual void damage(int weapon);

  void bounce(VECTOR2 &collisionVector, Entity &entity);
  void gravityForce(Entity *other, float frameTime);
};