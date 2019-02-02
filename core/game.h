#pragma once
#define WIN32_LEAN_AND_MEAN

// clang-format off
#include <windows.h>
#include <Mmsystem.h>
// clang-format on

#include <sstream>
#include <string>

#include "common.h"
#include "gameError.h"
#include "graphics.h"
#include "input.h"
#include "text.h"
#include "ecs.h"
#include "systems/physics.h"
#include "systems/renderable.h"

namespace gameNS
{
const char FONT[] = "Consolas";
const int POINT_SIZE = 15;
const COLOR_ARGB FONT_COLOR = graphicsNS::WHITE;
const int LOG_HEIGHT = 47;
} // namespace gameNS

class Game
{
protected:
  Graphics* graphics;
  Input* input;
  HWND hwnd;
  HRESULT hr;

  // Performance
  LARGE_INTEGER timeStart;
  LARGE_INTEGER timeEnd;
  LARGE_INTEGER timeFreq;

  float frameTime;
  float fps;
  Text gameText;
  DWORD sleepTime; // Sleep in between frames
  bool paused;
  bool initialised;

  // Debug
  bool showFps = false;
  bool debug = false;
  std::stringstream logBuffer;

  // ECS
  ECS ecs;
  SystemList gameSystems;
  SystemList graphicsSystems;

public:
  Game();
  virtual ~Game();

  Graphics* getGraphics() { return graphics; }
  Input* getInput() { return input; }

  // Handle input
  void handleInput(WPARAM, LPARAM, UINT);

  template <typename T> inline void debugLog(T content)
  {
    logBuffer << content << "\n";
    updateLog();
  }

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

private:
  void updateLog()
  {
    // Count lines in buffer
    std::string countString = logBuffer.str();
    std::string delimiter = "\n";
    size_t pos = 0;
    int lineCount = 0;

    while ((pos = countString.find(delimiter)) != -1) {
      lineCount++;
      countString.erase(0, pos + delimiter.length());
    }

    // Scroll buffer
    if (lineCount >= gameNS::LOG_HEIGHT) {
      std::string tempBuffer = logBuffer.str();
      tempBuffer.erase(0, tempBuffer.find(delimiter) + delimiter.length());
      logBuffer.str(tempBuffer);
    }
  }
};