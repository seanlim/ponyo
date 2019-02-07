#pragma once

#include "breakout.h"
#include "game.h"
#include "splash.h"

class Main : public Game
{
  Breakout* breakout = new Breakout();
  SplashScreen* splash = new SplashScreen();

public:
  void setupRootScene()
  {
    breakout->collisionSystem = collisionSystem;
    splash->breakout = breakout;
    this->setScene(splash);
  }
};