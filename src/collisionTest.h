#pragma once
#define WIN32_LEAN_AND_MEAN

#include "common.h"
#include "constants.h"
#include "ecs.h"
#include "math/random.h"
#include "scene.h"
#include "textureManager.h"

// Collision stress test
struct CCollisionTest : public Component<CCollisionTest> {
};

class SCollisionTest : public System
{
public:
  SCollisionTest()
  {
    System::addComponentType(CCollidable::id);
    System::addComponentType(CSprite::id);
    System::addComponentType(CCollisionTest::id);
  }

  virtual void updateComponents(float delta, BaseComponent** components)
  {
    CCollidable* collider = (CCollidable*)components[0];
    CSprite* sprite = (CSprite*)components[1];
    CCollisionTest* collisionTest = (CCollisionTest*)components[2];

    sprite->animates = collider->colliding;
  }
};

class CollisionTest : public Scene
{
  TextureManager ballTexture;

  SCollisionTest* collisionTestSystem;

  CSprite ballSprite;
  CMotion ballMotion;
  CCollidable ballCollider;
  CCollisionTest ballCollisionTest;

  float timer = 0;

  Array<EntityHook> ballEntities;

public:
  CollisionTest() : Scene() {}
  ~CollisionTest()
  {
    ballTexture.onLostDevice();
    ballTexture.onResetDevice();
  }

  void setupSystems() { collisionTestSystem = new SCollisionTest(); }

  void setupTextures()
  {
    if (!ballTexture.initialise(graphics, PADDLE_BALL))
      Logger::error("Failed to load ball texture");
  }

  void setupComponents()
  {
    ballSprite.startFrame = 0, ballSprite.endFrame = 7,
    ballSprite.currentFrame = 0;
    ballSprite.initialise(32, 32, 4, &ballTexture);
    ballSprite.setScale(0.6);
    ballSprite.setPosition(GAME_WIDTH / 2, GAME_HEIGHT / 2);
    ballSprite.animates = false;
    ballMotion.velocity = Vec2(0.0, 200);
    ballMotion.friction = 0.0;
    ballCollider.collisionType = CIRCLE;
    ballCollider.collisionResponse = BOUNCE;
  }

  void render() {}

  void update(float delta)
  {
    timer += 0.01f / delta;
    if (timer >= 450) {
      timer = 0;
      ballMotion.velocity = Vec2(randInt(100, 200), randInt(100, 200));
      ballEntities.push_back(ecs->makeEntity(ballSprite, ballMotion,
                                             ballCollider, ballCollisionTest));
    }
  }

  void attach()
  {
    gameSystems->addSystem(*collisionTestSystem);
    Scene::attach();
  }
  void detach()
  {
    for (int i = 0; i < ballEntities.size(); i++)
      ecs->removeEntity(ballEntities[i]);
    ballEntities.clear();
    gameSystems->removeSystem(*collisionTestSystem);
    Scene::detach();
  }
};