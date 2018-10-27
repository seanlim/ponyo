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

void Game::renderGame()
{
    if (SUCCEEDED(this->graphics->beginScene()))
    {
        this->render();
        this->graphics->endScene();
    }

    this->handleLostGraphicsDevice();

    this->graphics->showBackbuffer();
}

void Game::run(HWND hwnd)
{
    if (this->graphics == NULL)
        return;

    // Calculate elapsed time since last frame
    QueryPerformanceCounter(&timeEnd);
    this->frameTime = (float)(timeEnd.QuadPart - timeStart.QuadPart) / (float)timeFreq.QuadPart;

    if (this->frameTime < MIN_FRAME_TIME)
    {
        this->sleepTime = (DWORD)((MIN_FRAME_TIME - this->frameTime) * 1000);
        timeBeginPeriod(1);
        Sleep(this->sleepTime);
        timeEndPeriod(1);
        return;
    }

    if (this->frameTime > 0.0)
        this->fps = (this->fps * 0.99f) + (0.01f / this->frameTime);

    if (this->frameTime > MAX_FRAME_TIME)
        this->frameTime = MAX_FRAME_TIME;

    this->timeStart = this->timeEnd;

    if (!paused)
    {
        this->update();
        this->ai();
        this->collisions();
    }

    this->renderGame();
}

void Game::releaseAll()
{
}

void Game::resetAll()
{
}

void Game::deleteAll()
{
    this->releaseAll();
    safeDelete(this->graphics);

    initialised = false;
}