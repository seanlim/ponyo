#pragma once
#define WIN32_LEAN_AND_MEAN

#include "common.h"
#include "ecs.h"
#include "game.h"
#include "textureManager.h"
#include "tileMap.h"

class Breakout : public Game
{
  TextureManager tileTexture;

public:
  Breakout();
  virtual ~Breakout();
  void initialise(HWND);
  void update() {}
  void ai() {}
  void collisions() {}
  void render() {}
  void releaseAll();
  void resetAll();
};