#define _CRTDBG_MAP_ALLOC
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h>
#include <crtdbg.h>

#include "graphics.h"
#include "random.h"

bool AnotherInstance()
{
    HANDLE ourMutex = CreateMutex(NULL, true, "process0");
    return GetLastError() == ERROR_ALREADY_EXISTS;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
bool CreateMainWindow(HWND &, HINSTANCE, int);
LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hinst;
Graphics *graphics;

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
#if defined(DEBUG) | defined(DEBUG)
    _crtDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    MSG msg;
    HWND hwnd = NULL;

    if (AnotherInstance())
        return false;
    if (!CreateMainWindow(hwnd, hInstance, nCmdShow))
        return false;

    try
    {
        graphics = new Graphics();
        graphics->initialise(hwnd, GAME_WIDTH, GAME_HEIGHT, FULLSCREEN);

        int done = 0;
        while (!done)
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                // Look for quit message
                if (msg.message == WM_QUIT)
                    done = 1;

                // Decode and pass messages on to WinProc
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                graphics->showBackbuffer();
            }
        }
        safeDelete(graphics);
        return msg.wParam;
    }
    catch (const GameError &err)
    {
        MessageBox(NULL, err.getMessage(), "Error", MB_OK);
    }
    catch (...)
    {
        MessageBox(NULL, "Something is not quite right...", "Error", MB_OK);
    }

    safeDelete(graphics);
    return 0;
}

LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    short nVirtKey;
    const short PRESSED = (short)0x8000;
    TEXTMETRIC tm;
    DWORD chWidth = 40;
    DWORD chHeight = 40;

    switch (msg)
    {
    case WM_CREATE:
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_CHAR:
        switch (wParam)
        {
        case VK_SPACE:
            graphics->setBackColor(D3DCOLOR_XRGB(randInt(0, 255), randInt(0, 255), randInt(0, 255)));
        }
    default:
        DefWindowProc(hWnd, msg, wParam, lParam);
    }
}

bool CreateMainWindow(HWND &hwnd, HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX wcx;

    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc = WinProc;
    wcx.cbClsExtra = 0;
    wcx.cbWndExtra = 0;
    wcx.hInstance = hInstance;
    wcx.hIcon = NULL;
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcx.lpszMenuName = NULL;
    wcx.lpszClassName = CLASS_NAME;
    wcx.hIconSm = NULL;

    // Register the window class.
    // RegisterClassEx returns 0 on error.
    if (RegisterClassEx(&wcx) == 0) // if error
        return false;

    DWORD windowStyle;
    if (FULLSCREEN)
        windowStyle = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;
    else
        windowStyle = WS_OVERLAPPEDWINDOW;

    // Create window
    hwnd = CreateWindow(
        CLASS_NAME,
        GAME_TITLE,
        windowStyle,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        GAME_WIDTH,
        GAME_HEIGHT,
        (HWND)NULL,
        (HMENU)NULL,
        hInstance,
        (LPVOID)NULL);

    if (!hwnd)
        return false;

    if (!FULLSCREEN)
    {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        MoveWindow(hwnd, 0, 0, GAME_WIDTH + (GAME_WIDTH - clientRect.right), GAME_HEIGHT + (GAME_HEIGHT - clientRect.bottom), true);
    }

    ShowWindow(hwnd, nCmdShow);

    UpdateWindow(hwnd);
    return true;
}
