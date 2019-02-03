#pragma once

#include "common.h"
#include "ecs.h"
#include "textureManager.h"

// Simple Tilemap system implementation for breakout
#define marginX 480
#define rows 7
#define columns 10
#define tileSize 32

// Initialising CBreakOutTile as a CSprite and registering its id
// under SBreakOutTileMap causes double draws
// TODO: find a more elegant way around this
struct CBreakOutTile : public Component<CBreakOutTile> {
public:
  CSprite sprite;
};
class SBreakOutTileMap : public System
{
private:
  Graphics* graphics;
  int tileMap[columns][rows]; // row, column
public:
  SBreakOutTileMap(HWND _hwnd, uint32 width, uint32 height, bool fullscreen,
                   Graphics* _graphics)
      : System()
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

        breakoutTile->sprite.setPosition((marginX + (column * tileSize)),
                                         (row * tileSize));

        if (tileMap[row][column] == 1) {
          graphics->spriteBegin();
          breakoutTile->sprite.spriteData.texture =
              breakoutTile->sprite.textureManager->getTexture();
          graphics->drawSprite(breakoutTile->sprite.spriteData);
          graphics->spriteEnd();
        }
      }
    }
  }
};
