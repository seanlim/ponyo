#pragma once
#define WIN32_LEAN_AND_MEAN

#include "game.h"
#include "textureManager.h"
#include "image.h"

class SpaceWar : public Game
{
  TextureManager planetTexture;
  TextureManager nebulaTexture;
  TextureManager spaceShipTexture;

  Image planetImage;
  Image nebulaImage;
  Image spaceShipImage;

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
  void run(HWND);
};
