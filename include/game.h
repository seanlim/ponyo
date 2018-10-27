#ifndef _GAME_H
#define _GAME_H
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <mmsystem.h>

#include "graphics.h"
// #include "input.h"
#include "constants.h"
#include "gameError.h"

class Game
{
protected:
  Graphics *graphics;
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

  // Handle input
  // TODO: not sure how I want to implement this just yet
  void input();

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
#endif