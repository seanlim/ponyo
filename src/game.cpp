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

void Game::input(WPARAM wParam, LPARAM lParam, UINT msg)
{
    if (initialised)
    {
    }

    return;
}

void Game::initialise(HWND _hwnd)
{
    // Does the window handle need to be passed into the game?
    this->hwnd = _hwnd;

    this->graphics = new Graphics();
    this->graphics->initialise(this->hwnd, GAME_WIDTH, GAME_HEIGHT, FULLSCREEN);

    if (!QueryPerformanceFrequency(&timeFreq))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initialising high res timer"));
    QueryPerformanceCounter(&timeStart);

    initialised = true;
    return;
}
