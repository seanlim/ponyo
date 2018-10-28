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
    OutputDebugString(this->input->getTextIn().c_str());
    OutputDebugStringW(L"\n");
    if (this->input->getTextIn() == "exit")
    {
    }
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