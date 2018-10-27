#ifndef _SPACEWAR_H
#define _SPACEWAR_H
#define WIN32_LEAN_AND_MEAN

#include "game.h"

class SpaceWar : public Game
{
  public:
    SpaceWar();
    virtual ~SpaceWar();
    void initialise(HWND hwnd);
    void update();
    void ai();
    void collisions();
    void render();
    void releaseAll();
    void resetAll();
};
#endif