#include "input.h"

Input::Input()
{
    for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
    {

        this->keysDown[i] = false;
        this->keysPressed[i] = false;
    }
    this->newLine = true;
    this->textIn = "";
    this->charIn = 0;

    this->mouseX, this->mouseY, this->mouseRawX, this->mouseRawY = 0;

    this->mouseLButton, this->mouseMButton, this->mouseRButton, this->mouseX1Button, this->mouseX2Button = false;

    for (int i = 0; i < MAX_CONTROLLERS; i++)
        this->controllers[i].vibrateTimeLeft, this->controllers[i].vibrateTimeRight = 0;
}

Input::~Input()
{
    if (this->mouseCaptured)
        ReleaseCapture();
}

void Input::initialise(HWND hwnd, bool capture)
{
    try
    {
        this->mouseCaptured = capture;

        this->Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
        this->Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
        this->Rid[0].dwFlags = RIDEV_INPUTSINK;
        this->Rid[0].hwndTarget = hwnd;

        RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

        if (this->mouseCaptured)
            SetCapture(hwnd);

        // Clear controller memory
        ZeroMemory(this->controllers, sizeof(ControllerState), *MAX_CONTROLLERS);
        this->checkControllers();
    }
    catch (...)
    {
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initialising input system"));
    }
}

void Input::keyDown(WPARAM wParam)
{
    if (wParam < inputNS::KEYS_ARRAY_LEN)
    {
        this->keysDown[wParam] = true;
        this->keysPressed[wParam] = true;
    }
}

void Input::keyUp(WPARAM wParam)
{
    if (wParam < inputNS::KEYS_ARRAY_LEN)
        this->keysDown[wParam] = false;
}

void Input::keyIn(WPARAM wParam)
{
    if (this->newLine)
    {
        this->textIn.clear();
        this->newLine = false;
    }

    if (wParam == '\b' && this->textIn.length() > 0)
    {
        this->textIn.erase(this->textIn.size() - 1);
    }
    else
    {
        this->textIn += wParam;
        this->charIn = wParam;
    }

    this->newLine = (char)wParam == '/r';
}

bool Input::isKeyDown(UCHAR vKey) const
{
    if (vKey < inputNS::KEYS_ARRAY_LEN)
        return this->keysDown[vKey];
    else
        return false;
}

bool Input::wasKeyPressed(UCHAR vKey) const
{
    if (vKey < inputNS::KEYS_ARRAY_LEN)
        return this->keysPressed[vKey];
    return false;
}

bool Input::anyKeyPressed() const
{
    for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
    {
        if (this->keysPressed[i])
            return true;
    }
    return false;
}

void Input::clearKeyPress(UCHAR vKey)
{
    if (vKey < inputNS::KEYS_ARRAY_LEN)
        this->keysPressed[vKey] = false;
}

void Input::clear(UCHAR vKey)
{
    using namespace inputNS;

    if (vKey & KEYS_DOWN)
    {
        for (size_t i = 0; i < KEYS_ARRAY_LEN; i++)
            this->keysDown[i] = false;
    }
    if (vKey & KEYS_PRESSED)
    {
        for (size_t i = 0; i < KEYS_ARRAY_LEN; i++)
            this->keysPressed[i] = false;
    }
    if (vKey & MOUSE)
    {
        this->mouseX, this->mouseY, this->mouseRawX, this->mouseRawY = 0;
    }
    if (vKey & TEXT_IN)
    {
        this->cleartextIn();
    }
}

void Input::mouseIn(LPARAM lParam)
{
    this->mouseX = GET_X_LPARAM(lParam);
    this->mouseY = GET_Y_LPARAM(lParam);
}

// High-def mouse
void Input::mouseRawIn(LPARAM lParam)
{
    UINT dwSize = 40;
    static BYTE lpb[40];

    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

    RAWINPUT *raw = (RAWINPUT *)lpb;

    if (raw->header.dwType == RIM_TYPEMOUSE)
    {
        this->mouseRawX = raw->data.mouse.lLastX;
        this->mouseRawY = raw->data.mouse.lLastY;
    }
}

void Input::checkControllers()
{
    for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
    {
        this->controllers[i].connected = XInputGetState(i, &controllers[i].state) == ERROR_SUCCESS;
    }
}

void Input::readControllers()
{
    for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
    {
        if (this->controllers[i].connected && XInputGetState(i, &controllers[i].state) == ERROR_DEVICE_NOT_CONNECTED)
            this->controllers[i].connected = false;
    }
}

void Input::vibrateControllers(float frameTime)
{
    for (int i = 0; i < MAX_CONTROLLERS; i++)
    {
        if (this->controllers[i].connected)
        {
            this->controllers[i].vibrateTimeLeft -= frameTime;
            if (this->controllers[i].vibrateTimeLeft < 0)
                this->gamePadVibrateLeft(i, 0, 0);
            this->controllers[i].vibrateTimeRight -= frameTime;
            if (this->controllers[i].vibrateTimeRight < 0)
                this->gamePadVibrateRight(i, 0, 0);
        }
    }
}
