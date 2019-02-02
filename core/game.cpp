#include "game.h"

Game::Game()
{
  this->initialised = false;
  this->graphics = NULL;
  this->input = new Input();
  this->paused = false;
  fps = 100;

  logBuffer << "Ponyo Engine 2019\n";
}

Game::~Game()
{
  this->deleteAll();
  ShowCursor(true);
}

void Game::handleInput(WPARAM wParam, LPARAM lParam, UINT msg)
{
  if (this->initialised) {
    switch (msg) {
    case WM_MOUSEMOVE:
      input->mouseIn(lParam);
      break;
    case WM_INPUT:
      input->mouseRawIn(lParam);
      break;
    case WM_LBUTTONDOWN:
      input->setMouseLButton(true);
      input->mouseIn(lParam);
      break;
    case WM_LBUTTONUP:
      input->setMouseLButton(false);
      input->mouseIn(lParam);
      break;
    case WM_MBUTTONDOWN:
      input->setMouseMButton(true);
      input->mouseIn(lParam);
      break;
    case WM_MBUTTONUP:
      input->setMouseMButton(false);
      input->mouseIn(lParam);
      break;
    case WM_RBUTTONDOWN:
      input->setMouseRButton(true);
      input->mouseIn(lParam);
      break;
    case WM_RBUTTONUP:
      input->setMouseRButton(false);
      input->mouseIn(lParam);
      break;
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
      input->setMouseXButton(wParam);
      input->mouseIn(lParam);
      break;
    case WM_DEVICECHANGE:
      input->checkControllers();
      break;
    default:
      break;
    }
  }

  return;
}

void Game::initialise(HWND _hwnd)
{
  debugLog("Setting up game...");

  this->hwnd = _hwnd;

  this->graphics = new Graphics();
  this->graphics->initialise(this->hwnd, GAME_WIDTH, GAME_HEIGHT, FULLSCREEN);

  this->input->initialise(hwnd, false);

  if (!QueryPerformanceFrequency(&timeFreq))
    throw(GameError(gameErrorNS::FATAL_ERROR,
                    "Error initialising high res timer"));
  QueryPerformanceCounter(&timeStart);

  debugLog("Setting up font...");

  // Init font
  if (gameText.initialise(graphics, gameNS::POINT_SIZE, false, false,
                          gameNS::FONT) == false)
    throw(
        GameError(gameErrorNS::FATAL_ERROR, "Failed to initialise game text"));

  gameText.setFontColor(gameNS::FONT_COLOR);

  // Init graphics
  SRenderable* renderSystem = new SRenderable(
      this->hwnd, GAME_WIDTH, GAME_HEIGHT, FULLSCREEN, this->graphics);
  graphicsSystems.addSystem(*renderSystem);

  // Init physics
  SPhysics* physicsSystem = new SPhysics();
  gameSystems.addSystem(*physicsSystem);

  initialised = true;
  return;
}

void Game::handleLostGraphicsDevice()
{
  this->hr = this->graphics->getDeviceState();
  if (FAILED(this->hr)) {
    switch (this->hr) {
    case D3DERR_DEVICELOST:
      Sleep(100);
      return;
    case D3DERR_DEVICENOTRESET:
      this->releaseAll();
      this->hr = this->graphics->reset();
      if (FAILED(hr)) return;
      this->resetAll();
    default:
      return;
    }
  }
}

void Game::renderGame()
{
  const int BUF_SIZE = 20;
  static char buffer[BUF_SIZE];

  if (SUCCEEDED(this->graphics->beginScene())) {
    // Call graphics system
    ecs.updateSystems(graphicsSystems, frameTime);

    // Draw HUD
    graphics->spriteBegin();
    if (showFps) {
      _snprintf_s(buffer, BUF_SIZE, "%d FPS", (int)fps);
      gameText.print(buffer, GAME_WIDTH - 200, GAME_HEIGHT - gameNS::POINT_SIZE,
                     DT_RIGHT);
    }
    if (debug) {
      gameText.print(logBuffer.str(), 0, 0, DT_LEFT);
    }
    graphics->spriteEnd();
    this->graphics->endScene();
  }

  this->handleLostGraphicsDevice();

  this->graphics->showBackbuffer();
}

void Game::run(HWND hwnd)
{
  if (this->graphics == NULL) return;

  // Calculate elapsed time since last frame
  QueryPerformanceCounter(&timeEnd);
  this->frameTime =
      (float)(timeEnd.QuadPart - timeStart.QuadPart) / (float)timeFreq.QuadPart;

  if (this->frameTime < MIN_FRAME_TIME) {
    this->sleepTime = (DWORD)((MIN_FRAME_TIME - this->frameTime) * 1000);
    timeBeginPeriod(1);
    Sleep(this->sleepTime);
    timeEndPeriod(1);
    return;
  }

  if (this->frameTime > 0.0)
    this->fps = (this->fps * 0.99f) + (0.01f / this->frameTime);

  if (this->frameTime > MAX_FRAME_TIME) this->frameTime = MAX_FRAME_TIME;

  this->timeStart = this->timeEnd;

  for (auto x : input->getActiveGameCommands()) {
    switch (x) {
    case GameCommands::toggleFPS:
      Game::showFps = !Game::showFps;
      debugLog("Toggle FPS");
      break;
    case GameCommands::toggleDebug:
      Game::debug = !Game::debug;
      break;
    case GameCommands::Quit:
      ExitProcess(0);
      break;
    }
  }

  if (!paused) {
    // TEMP UPDATE PHYSICS
    // move this to somewhere more intentional
    ecs.updateSystems(gameSystems, frameTime);
    this->update();
    this->ai();
    this->collisions();
    this->input->vibrateControllers(frameTime);
  }

  this->renderGame();

  this->input->readControllers();
  this->input->pollKeys();

  debugLog(frameTime);
}

void Game::releaseAll() {}

void Game::resetAll() {}

void Game::deleteAll()
{
  this->releaseAll();
  safeDelete(this->graphics);
  safeDelete(this->input);

  initialised = false;
}