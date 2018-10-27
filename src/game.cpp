#include "game.h"

Game::Game()
{
    this->initialised = false;
    this->graphics = NULL;
    this->paused = false;
}

Game::~Game()
{
    this->deleteAll();
    ShowCursor(true);
}

//TODO: temporarily leaving out inputHandler implementation
void Game::input()
{
}
