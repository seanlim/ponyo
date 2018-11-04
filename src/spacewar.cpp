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

    // load textures
    if (!nebulaTexture.initialise(graphics, NEBULA_IMAGE))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initialising nebula texture"));
    if (!planetTexture.initialise(graphics, PLANET_IMAGE))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initialising planet texture"));
    // nebula
    if (!nebulaImage.initialise(graphics, 0, 0, 0, &nebulaTexture))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing nebula"));
    nebulaImage.setX(0.0);
    nebulaImage.setY(0.0);
    // planet
    if (!planetImage.initialise(graphics, 0, 0, 0, &planetTexture))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing planet"));
    // place planet in center of screen
    planetImage.setX(GAME_WIDTH * 0.5f - planetImage.getWidth() * 0.5f);
    planetImage.setY(GAME_HEIGHT * 0.5f - planetImage.getHeight() * 0.5f);

    if (!nebulaImage.initialise(graphics, 0, 0, 0, &nebulaTexture))
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
    this->graphics->spriteBegin();
    this->nebulaImage.draw();
    this->planetImage.draw();
    this->graphics->spriteEnd();
}

void SpaceWar::releaseAll()
{
    this->planetTexture.onLostDevice();
    this->nebulaTexture.onLostDevice();
    Game::releaseAll();
    return;
}

void SpaceWar::resetAll()
{
    this->planetTexture.onResetDevice();
    this->nebulaTexture.onResetDevice();

    Game::resetAll();
    return;
}

void SpaceWar::run(HWND hwnd)
{
    Game::run(hwnd);
    return;
}
