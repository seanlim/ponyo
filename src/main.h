#pragma once

#include "breakout.h"
#include "collisionTest.h"
#include "game.h"
#include "splash.h"

class Main : public Game
{
  Breakout* breakout = new Breakout();
  SplashScreen* splash = new SplashScreen();
  CollisionTest* collisionTest = new CollisionTest();

public:
  void setupRootScene()
  {
    breakout->collisionSystem = collisionSystem;
    this->setScene(collisionTest);
  }

  void nextScene(Scene* currentScene)
  {
    // if (currentScene == splash) this->setScene(breakout);
  }
};