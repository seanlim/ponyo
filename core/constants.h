#pragma once
#define WIN_32_LEAN_AND_MEAN

#include <windows.h>

const double PI = 3.14159265;
const float FRAME_RATE = 200.0f;
const float MIN_FRAME_RATE = 10.0f;
const float MIN_FRAME_TIME = 1.0f / FRAME_RATE;
const float MAX_FRAME_TIME = 1.0f / MIN_FRAME_RATE;
const char CLASS_NAME[] = "Spacewar";
const char GAME_TITLE[] = "Game Engine";
const bool FULLSCREEN = false; // windowed or fullscreen
const UINT GAME_WIDTH = 640;   // width of game in pixels
const UINT GAME_HEIGHT = 480;  // height of game in pixels

// Safely delete pointer referenced item
#define safeDelete(ptr)   \
    {                     \
        if (ptr)          \
        {                 \
            delete (ptr); \
            (ptr) = NULL; \
        }                 \
    }
// Safely release pointer referenced item
#define safeRelease(ptr)      \
    {                         \
        if (ptr)              \
        {                     \
            (ptr)->Release(); \
            (ptr) = NULL;     \
        }                     \
    }
// Safely delete pointer referenced array
#define safeDeleteArr(ptr) \
    {                      \
        if (ptr)           \
        {                  \
            delete[](ptr); \
            (ptr) = NULL;  \
        }                  \
    }

// Safely call onLostDevice
#define SAFE_ON_LOST_DEVICE(ptr) \
    {                            \
        if (ptr)                 \
        {                        \
            ptr->onLostDevice(); \
        }                        \
    }
// Safely call onResetDevice
#define SAFE_ON_RESET_DEVICE(ptr) \
    {                             \
        if (ptr)                  \
        {                         \
            ptr->onResetDevice(); \
        }                         \
    }
#define TRANSCOLOR SETCOLOR_ARGB(0, 255, 0, 255) // transparent color (magenta)

// TODO: move somewhere else
const byte ESC_KEY = VK_ESCAPE; // escape key

// image resources
const char NEBULA_IMAGE[] = "pictures\\orion.jpg";  // photo source NASA/courtesy of nasaimages.org
const char PLANET_IMAGE[] = "pictures\\planet.png"; // picture of planet
const char SHIP_IMAGE[] = "pictures\\ship.png";     // spaceship
