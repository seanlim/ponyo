#pragma once

#include "common.h"
#include "ecs.h"
#include "graphics.h"
#include "textureManager.h"

struct CDrawable : public Component<CDrawable> {
  SpriteData spriteData;
  TextureManager* textureManager;

  int startFrame, endFrame, currentFrame;
  float frameDelay;
  float animTimer;
  COLOR_ARGB colorFilter;

  bool loops;
  bool visible;

  void initialise(int width, int height, int ncols, TextureManager* textureM)
  {
    this->spriteData.width = 2, this->spriteData.height = 2;
    spriteData.x, this->spriteData.y = 0.0;
    spriteData.scale = 1.0;
    spriteData.angle = 0.0;
    spriteData.rect.top = 0;
    spriteData.rect.right = this->spriteData.width;
    spriteData.rect.bottom = this->spriteData.height;
    spriteData.texture = NULL;
    spriteData.flipHorizontal = false, this->spriteData.flipVertical = false;

    startFrame = 0, endFrame = 0, currentFrame = 0;
    frameDelay = 1.0;
    animTimer = 0.0;
    visible = true;
    loops = true;
    colorFilter = graphicsNS::WHITE;
    textureManager = NULL;

    textureManager = textureM;
    spriteData.texture = textureManager->getTexture();

    this->spriteData.width = width;
    if (width == 0) this->spriteData.width = this->textureManager->getWidth();
    this->spriteData.height = height;
    if (height == 0)
      this->spriteData.height = this->textureManager->getHeight();

    int cols = ncols;
    if (ncols == 0) cols = 1;

    // configure spriteData.rect to draw currentFrame
    spriteData.rect.left = (currentFrame % cols) * spriteData.width;
    // right edge + 1
    spriteData.rect.right = spriteData.rect.left + spriteData.width;
    spriteData.rect.top = (currentFrame / cols) * spriteData.height;
    // bottom edge + 1
    spriteData.rect.bottom = spriteData.rect.top + spriteData.height;
  }
};

class SDrawable : public System
{
  Graphics* graphics;

public:
  SDrawable(HWND _hwnd, uint32 width, uint32 height, bool fullscreen,
            Graphics* _graphics)
      : System()
  {
    System::addComponentType(CDrawable::id);

    this->graphics = _graphics;
  }
  virtual void updateComponents(float delta, BaseComponent** components)
  {
    CDrawable* drawable = (CDrawable*)components[0];

    if (drawable->visible) {
      graphics->spriteBegin();
      drawable->spriteData.texture = drawable->textureManager->getTexture();
      graphics->drawSprite(drawable->spriteData);
      graphics->spriteEnd();
    }
  }
};
