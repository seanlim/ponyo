#include "spacewar.h"

SpaceWar::SpaceWar()
{
}

SpaceWar::~SpaceWar()
{
    this->releaseAll();
}

void SpaceWar::initialise(HWND hwnd)
{
    Game::initialise(hwnd);

    return;
}

void SpaceWar::update()
{
    if (this->input->getTextIn() == "exit")
    {
        PostQuitMessage(0);
    }

    if ((bool)this->input->getMouseLButton())
        this->graphics->setBackColor(SETCOLOR_ARGB(255, 128, 50, 50));
    else if ((bool)this->input->getMouseMButton())
        this->graphics->setBackColor(SETCOLOR_ARGB(255, 50, 50, 128));
    else if ((bool)this->input->getMouseRButton())
        this->graphics->setBackColor(SETCOLOR_ARGB(255, 50, 128, 50));
}

void SpaceWar::ai()
{
}

void SpaceWar::collisions()
{
}

void SpaceWar::render()
{
}

void SpaceWar::releaseAll()
{
    Game::releaseAll();
    return;
}

void SpaceWar::resetAll()
{
    Game::resetAll();
    return;
}

void SpaceWar::run(HWND hwnd)
{
    Game::run(hwnd);
    return;
}
