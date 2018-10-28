#include "graphics.h"

Graphics::Graphics()
{
    this->direct3d, device3d = NULL;
    this->fullscreen = false;
    this->width = GAME_WIDTH;
    this->height = GAME_HEIGHT;
    this->setBackColor(SETCOLOR_ARGB(255, 0, 0, 128));
}

void Graphics::initD3Dpp()
{
    try
    {
        ZeroMemory(&d3dpp, sizeof(d3dpp));

        d3dpp.BackBufferWidth = width;
        d3dpp.BackBufferHeight = height;

        // Switch to 24-bit color on fullscreen
        d3dpp.BackBufferFormat = fullscreen ? D3DFMT_X8R8G8B8 : D3DFMT_UNKNOWN;
        d3dpp.BackBufferCount = 1;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow = hwnd;
        d3dpp.Windowed = !fullscreen;
        d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    }
    catch (...)
    {
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initialising D3D params"));
    }
}

void Graphics::initialise(HWND hw, int w, int h, bool fs)
{
    hwnd = hw;
    width = w;
    height = h;
    fullscreen = fs;

    // Init Direct3D
    direct3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (direct3d == NULL)
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initialising Direct3D"));

    initD3Dpp();

    // determine if graphics card supports harware texturing and lighting and vertex
    // shaders
    D3DCAPS9 caps;
    DWORD behavior;
    result = direct3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
    // If device doesn't support HW T&L or doesn't support 1.1 vertex
    // shaders in hardware, then switch to software vertex processing.
    if ((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
        caps.VertexShaderVersion < D3DVS_VERSION(1, 1))
        behavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING; // use software only processing
    else
        behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING; // use hardware only processing

    //create Direct3D device
    result = direct3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hwnd,
        behavior,
        &d3dpp,
        &device3d);

    if (FAILED(result))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error creating Direct3D device"));
}

void Graphics::setBackColor(COLOR_ARGB c)
{
    this->backColor = c;
}

HRESULT Graphics::showBackbuffer()
{
    result = E_FAIL;

    result = device3d->Present(NULL, NULL, NULL, NULL);

    return result;
}

HRESULT Graphics::getDeviceState()
{
    this->result = E_FAIL;

    if (device3d == NULL)
        return this->result;

    this->result = this->device3d->TestCooperativeLevel();
    return result;
}

HRESULT Graphics::reset()
{
    result = E_FAIL;
    this->initD3Dpp();
    // Reset graphics device
    this->result = this->device3d->Reset(&d3dpp);
    return result;
}

HRESULT Graphics::beginScene()
{
    this->result = E_FAIL;
    if (this->device3d == NULL)
        return this->result;
    this->device3d->Clear(0, NULL, D3DCLEAR_TARGET, this->backColor, 1.0F, 0);
    this->result = this->device3d->BeginScene();
    return this->result;
}

HRESULT Graphics::endScene()
{
    this->result = E_FAIL;
    if (this->device3d)
        this->result = this->device3d->EndScene();
    return this->result;
}

// Destroy
Graphics::~Graphics()
{
    releaseAll();
}

void Graphics::releaseAll()
{
    safeRelease(device3d);
    safeRelease(direct3d);
}