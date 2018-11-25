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
    if (!spaceShipTexture.initialise(graphics, SHIP_IMAGE))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initialising spaceship texture"));

    if (!nebulaImage.initialise(graphics, 0, 0, 0, &nebulaTexture))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing nebula"));
    nebulaImage.setX(0.0);
    nebulaImage.setY(0.0);

    if (!planetImage.initialise(graphics, 0, 0, 0, &planetTexture))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing planet"));
    planetImage.setX(GAME_WIDTH * 0.5f - planetImage.getWidth() * 0.5f);
    planetImage.setY(GAME_HEIGHT * 0.5f - planetImage.getHeight() * 0.5f);

    if (!nebulaImage.initialise(graphics, 0, 0, 0, &nebulaTexture))
        return;

    // Animated sprite
    if (!spaceShipImage.initialise(graphics, 32, 32, 2, &spaceShipTexture))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing spaceship"));

    spaceShipImage.setX(GAME_WIDTH / 4);
    spaceShipImage.setY(GAME_HEIGHT / 4);

    spaceShipImage.setFrames(0, 3);
    spaceShipImage.setCurrentFrame(0);
    spaceShipImage.setFrameDelay(0.2f);
    spaceShipImage.setDegrees(45.0f);
}

void SpaceWar::update()
{
    if (input->isKeyDown(VK_RIGHT))
    {
        shipVelocity += 1.0f;
    }

    if (input->isKeyDown(VK_LEFT))
    {
        shipVelocity -= 1.0f;
    }

    spaceShipImage.setX(spaceShipImage.getX() + frameTime * shipVelocity);
    if (spaceShipImage.getX() > GAME_WIDTH)
    {
        spaceShipImage.setX(0.0);
    }
    else if (spaceShipImage.getX() < -spaceShipImage.getWidth())
    {
        spaceShipImage.setX(GAME_WIDTH);
    }

    if (input->isKeyDown(VK_UP))
    {
        spaceShipImage.setY(spaceShipImage.getY() - frameTime * 100.0f);
        if (spaceShipImage.getY() < -spaceShipImage.getHeight())
            spaceShipImage.setY((float)GAME_HEIGHT);
    }
    if (input->isKeyDown(VK_DOWN))
    {
        spaceShipImage.setY(spaceShipImage.getY() + frameTime * 100.0f);
        if (spaceShipImage.getY() > GAME_HEIGHT)
            spaceShipImage.setY((float)-spaceShipImage.getHeight());
    }

    // need to update ship with frameTime to swap frames (animate)
    spaceShipImage.update(frameTime);
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
    this->spaceShipImage.draw();
    this->graphics->spriteEnd();
}

void SpaceWar::releaseAll()
{
    this->planetTexture.onLostDevice();
    this->nebulaTexture.onLostDevice();
    this->spaceShipTexture.onLostDevice();
    Game::releaseAll();
    return;
}

void SpaceWar::resetAll()
{
    this->planetTexture.onResetDevice();
    this->nebulaTexture.onResetDevice();
    this->spaceShipTexture.onResetDevice();

    Game::resetAll();
    return;
}

void SpaceWar::run(HWND hwnd)
{
    Game::run(hwnd);
    return;
}
