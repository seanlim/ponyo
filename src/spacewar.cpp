#include "spacewar.h"

SpaceWar::SpaceWar()
{
}

SpaceWar::~SpaceWar()
{
    this->releaseALl();
}

void SpaceWar::initialise(HWND hwnd)
{
    Game::initialise(hwnd);

    return;
}

void SpaceWar::update()
{
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