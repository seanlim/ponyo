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
        switch (msg)
        {
            // TODO: implement with input class
        }
    }

    return;
}

void Game::initialise(HWND _hwnd)
{
    this->hwnd = _hwnd;

    this->graphics = new Graphics();
    this->graphics->initialise(this->hwnd, GAME_WIDTH, GAME_HEIGHT, FULLSCREEN);

    if (!QueryPerformanceFrequency(&timeFreq))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initialising high res timer"));
    QueryPerformanceCounter(&timeStart);

    initialised = true;
    return;
}

void Game::handleLostGraphicsDevice()
{
    this->hr = this->graphics->getDeviceState();
    if (FAILED(this->hr))
    {
        switch (this->hr)
        {
        case D3DERR_DEVICELOST:
            Sleep(100);
            return;
        case D3DERR_DEVICENOTRESET:
            this->releaseAll();
            this->hr = this->graphics->reset();
            if (FAILED(hr))
                return;
            this->resetAll();
        default:
            return;
        }
    }
}