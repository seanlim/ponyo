#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <Mmsystem.h>

#include "graphics.h"
#include "input.h"
#include "constants.h"
#include "gameError.h"

class Game
{
protected:
  Graphics *graphics;
  Input *input;
  HWND hwnd;
  HRESULT hr;

  // Performance
  LARGE_INTEGER timeStart;
  LARGE_INTEGER timeEnd;
  LARGE_INTEGER timeFreq;

  float frameTime;
  float fps;
  DWORD sleepTime; // Sleep in between frames
  bool paused;
  bool initialised;

public:
  Game();
  virtual ~Game();

  Graphics *getGraphics() { return graphics; }
  Input *getInput() { return input; }

  // Handle input
  void handleInput(WPARAM, LPARAM, UINT);

  virtual void initialise(HWND hwnd);
  virtual void run(HWND);
  virtual void releaseAll();
  virtual void resetAll();
  virtual void deleteAll();
  virtual void renderGame();
  virtual void handleLostGraphicsDevice();

  virtual void render() = 0;
  virtual void update() = 0;
  virtual void ai() = 0;
  virtual void collisions() = 0;

  void exitGame();
};
