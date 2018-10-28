#include "game.h"

Game::Game()
{
    this->initialised = false;
    this->graphics = NULL;
    this->input = new Input();
    this->paused = false;
}

Game::~Game()
{
    this->deleteAll();
    ShowCursor(true);
}

void Game::handleInput(WPARAM wParam, LPARAM lParam, UINT msg)
{
    if (this->initialised)
    {
        switch (msg)
        {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: // key down
            input->keyDown(wParam);
        case WM_KEYUP:
        case WM_SYSKEYUP: // key up
            input->keyUp(wParam);
        case WM_CHAR: // character entered
            input->keyIn(wParam);
        case WM_MOUSEMOVE: // mouse moved
            input->mouseIn(lParam);
        case WM_INPUT: // raw mouse data in
            input->mouseRawIn(lParam);
        case WM_LBUTTONDOWN: // left mouse button down
            input->setMouseLButton(true);
            input->mouseIn(lParam); // mouse position
        case WM_LBUTTONUP:          // left mouse button up
            input->setMouseLButton(false);
            input->mouseIn(lParam); // mouse position
        case WM_MBUTTONDOWN:        // middle mouse button down
            input->setMouseMButton(true);
            input->mouseIn(lParam); // mouse position
        case WM_MBUTTONUP:          // middle mouse button up
            input->setMouseMButton(false);
            input->mouseIn(lParam); // mouse position
        case WM_RBUTTONDOWN:        // right mouse button down
            input->setMouseRButton(true);
            input->mouseIn(lParam); // mouse position
        case WM_RBUTTONUP:          // right mouse button up
            input->setMouseRButton(false);
            input->mouseIn(lParam); // mouse position
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP: // mouse X button down/up
            input->setMouseXButton(wParam);
            input->mouseIn(lParam); // mouse position
            input->checkControllers();
        }
    }

    return;
}

void Game::initialise(HWND _hwnd)
{
    this->hwnd = _hwnd;

    this->graphics = new Graphics();
    this->graphics->initialise(this->hwnd, GAME_WIDTH, GAME_HEIGHT, FULLSCREEN);

    this->input->initialise(hwnd, false);

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
        this->input->vibrateControllers(frameTime);
    }

    this->renderGame();

    this->input->readControllers();

    // Clear input after all key checks are done
    this->input->clear(inputNS::KEYS_PRESSED);
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
    safeDelete(this->input);

    initialised = false;
}