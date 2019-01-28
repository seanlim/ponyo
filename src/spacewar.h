#pragma once
#define WIN32_LEAN_AND_MEAN

#include "drawable.h"
#include "ecs.h"
#include "game.h"
#include "image.h"
#include "planet.h"
#include "ship.h"
#include "systems/motion.h"
#include "textureManager.h"

class SpaceWar : public Game
{
  TextureManager nebulaTexture;
  TextureManager gameTexture;
  Planet planet;
  Ship ship1, ship2;
  Image nebulaImage;

  ECS ecs;
  SystemList mainSystems;

public:
  SpaceWar();
  virtual ~SpaceWar();
  void initialise(HWND);
  void update();
  void ai();
  void collisions();
  void render();
  void releaseAll();
  void resetAll();
};
