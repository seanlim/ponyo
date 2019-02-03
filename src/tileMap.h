#pragma once

#include "common.h"
#include "ecs.h"
#include "textureManager.h"

// Simple Tilemap system implementation for breakout
#define marginX 480
#define rows 7
#define columns 10
#define tileSize 32

struct CBreakOutTile : public CSprite {
  bool enabled;
};
class SBreakOutTileMap : public SRenderable
{
private:
  int tileMap[columns][rows]; // row, column
public:
  SBreakOutTileMap(HWND _hwnd, uint32 width, uint32 height, bool fullscreen,
                   Graphics* _graphics)
      : SRenderable(_hwnd, width, height, fullscreen, _graphics)
  {
    System::addComponentType(CBreakOutTile::id);
    this->graphics = _graphics;
    setTileMap(1);
  }

  inline void setTileMap(int value)
  {
    for (int row = 0; row < rows; row++) {
      for (int column = 0; column < columns; column++) {
        tileMap[row][column] = 1;
      }
    }
  }

  virtual void updateComponents(float delta, BaseComponent** components)
  {
    CBreakOutTile* breakoutTile = (CBreakOutTile*)components[0];
    for (int column = 0; column < columns; column++) {
      for (int row = 0; row < rows; row++) {

        breakoutTile->setPosition((marginX + (column * tileSize)),
                                  (row * tileSize));

        if (tileMap[row][column] == 1) {
          graphics->spriteBegin();
          breakoutTile->spriteData.texture =
              breakoutTile->textureManager->getTexture();
          graphics->drawSprite(breakoutTile->spriteData);
          graphics->spriteEnd();
        }
      }
    }
  }
};
