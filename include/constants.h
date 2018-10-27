#ifndef _CONSTANTS_H
#define _CONSTANTS_H
#define WIN_32_LEAN_AND_MEAN

#include <windows.h>
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

//Constants
const double PI = 3.14159265;
const float FRAME_RATE = 200.0f;
const float MIN_FRAME_RATE = 10.0f;
const float MIN_FRAME_TIME = 1.0f / FRAME_RATE;
const float MAX_FRAME_TIME = 1.0f / MIN_FRAME_RATE;
// window
const char CLASS_NAME[] = "Spacewar";
const char GAME_TITLE[] = "Game Engine";
const bool FULLSCREEN = false; // windowed or fullscreen
const UINT GAME_WIDTH = 640;   // width of game in pixels
const UINT GAME_HEIGHT = 480;  // height of game in pixels

// key mappings
// In this game simple constants are used for key mappings. If variables were used
// it would be possible to save and restore key mappings from a data file.
const byte ESC_KEY = VK_ESCAPE; // escape key

#endif
