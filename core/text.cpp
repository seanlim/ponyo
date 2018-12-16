#include "text.h"

Text::Text()
{
  color = SETCOLOR_ARGB(255, 255, 255, 255); // default to white font

  // set font position
  fontRect.top = 0;
  fontRect.left = 0;
  fontRect.right = GAME_WIDTH;
  fontRect.bottom = GAME_HEIGHT;
  dxFont = NULL;
  angle = 0;
}

Text::~Text() { safeRelease(dxFont); }
bool Text::initialise(Graphics* g, int height, bool bold, bool italic,
                      const std::string& fontName)
{
  this->graphics = g;
  UINT weight = (bold) ? FW_BOLD : FW_NORMAL;

  // Create Font
  if (FAILED(D3DXCreateFont(graphics->get3DDevice(), height, 0, weight, 1,
                            italic, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                            fontName.c_str(), &dxFont)))
    return false;

  // Transformation matrix
  D3DXMatrixTransformation2D(&matrix, NULL, 0.0f, NULL, NULL, 0.0f, NULL);
  return true;
}

int Text::print(const std::string& str, int x, int y)
{
  if (dxFont == NULL) return 0;
  // set font position
  fontRect.top = y;
  fontRect.left = x;

  // Rotation center
  D3DXVECTOR2 rCenter = D3DXVECTOR2((float)x, (float)y);
  // Setup matrix to rotate text by angle
  D3DXMatrixTransformation2D(&matrix, NULL, 0.0f, NULL, &rCenter, angle, NULL);
  // Tell the sprite about the matrix "Hello Neo"
  graphics->getSprite()->SetTransform(&matrix);
  return dxFont->DrawText(graphics->getSprite(), str.c_str(), -1, &fontRect,
                          DT_LEFT, color);
}

int Text::print(const std::string& str, int x, int y, UINT format)
{
  if (dxFont == NULL) return 0;
  // set font position
  fontRect.top = y;
  fontRect.left = x;

  // Rotation center
  D3DXVECTOR2 rCenter = D3DXVECTOR2((float)x, (float)y);
  // Setup matrix to rotate text by angle
  D3DXMatrixTransformation2D(&matrix, NULL, 0.0f, NULL, &rCenter, angle, NULL);
  // Tell the sprite about the matrix "Hello Neo"
  graphics->getSprite()->SetTransform(&matrix);
  return dxFont->DrawText(graphics->getSprite(), str.c_str(), -1, &fontRect,
                          format, color);
}

void Text::onLostDevice()
{
  if (dxFont == NULL) return;
  // dxFont->onLostDevice();
}

void Text::onResetDevice()
{

  if (dxFont == NULL) return;
  // dxFont->onResetDevice();
}