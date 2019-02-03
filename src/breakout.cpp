#include "breakout.h"

Breakout::Breakout() {}

Breakout::~Breakout() { this->releaseAll(); }

void Breakout::initialise(HWND hwnd)
{
  Game::initialise(hwnd);

  // Tile map system
  SBreakOutTileMap* tileMapSystem = new SBreakOutTileMap(
      this->hwnd, GAME_WIDTH, GAME_HEIGHT, FULLSCREEN, this->graphics);
  graphicsSystems.addSystem(*tileMapSystem);

  // Load textures
  if (!tileTexture.initialise(graphics, TILE_IMAGE))
    Logger::error("Failed to load tile textures");

  // Init tile sprite
  CBreakOutTile tileSprite;
  tileSprite.initialise(64, 64, 1, &tileTexture);
  tileSprite.startFrame = 0;
  tileSprite.endFrame = 0;
  tileSprite.currentFrame = 0;
  tileSprite.spriteData.scale = 0.5;

  ecs.makeEntity(tileSprite);

  return;
}

void Breakout::resetAll()
{
  this->tileTexture.onResetDevice();
  Game::resetAll();
  return;
}
void Breakout::releaseAll()
{
  this->tileTexture.onLostDevice();
  Game::releaseAll();
  return;
}