#ifndef _GRAPHICS_H
#define _GRAPHICS_H
#define WIN32_LEAN_AND_MEAN

#ifndef _DEBUG
#define D3D_DEBUG_INFO
#endif

#include <d3d9.h>
#include "constants.h"
#include "gameError.h"

// DirectX
#define LP_3DDEVICE LPDIRECT3DDEVICE9
#define LP_3D LPDIRECT3D9

// Colors
#define COLOR_ARGB DWORD
#define SETCOLOR_ARGB(a, r, g, b) \
  ((COLOR_ARGB)((((a)&0xff) << 24) | (((r)&0xff) << 16) | (((g)&0xff) << 8) | ((b)&0xff)))

class Graphics
{
private:
  LP_3D direct3d;
  LP_3DDEVICE device3d;
  D3DPRESENT_PARAMETERS d3dpp;

  HRESULT result;
  HWND hwnd;
  COLOR_ARGB backColor = D3DCOLOR_XRGB(0, 0, 128);
  bool fullscreen;
  int width, height;

  void initD3Dpp();

public:
  Graphics();
  virtual ~Graphics();

  LP_3D get3D() { return direct3d; }
  LP_3DDEVICE get3DDevice() { return device3d; }
  HDC getDC() { return GetDC(hwnd); }

  void setBackColor(COLOR_ARGB c);
  void releaseAll();
  void initialise(HWND hw, int width, int height, bool fullscreen);

  HRESULT showBackbuffer();
  HRESULT reset();
  HRESULT getDeviceState();
};

#endif