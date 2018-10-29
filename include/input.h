#pragma once

#include <windows.h>
#include <windowsX.h>
#include <string>
#include <XInput.h>

#include "constants.h"
#include "gameError.h"

#pragma comment(lib, "xinput.lib")

// High-definition mouse
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT)0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT)0x02)
#endif

namespace inputNS
{
const int KEYS_ARRAY_LEN = 256;

const UCHAR KEYS_DOWN = 1;
const UCHAR KEYS_PRESSED = 2;
const UCHAR MOUSE = 4;
const UCHAR TEXT_IN = 8;

const UCHAR KEYS_MOUSE_TEXT = KEYS_DOWN + KEYS_PRESSED + MOUSE + TEXT_IN;

} // namespace inputNS

const DWORD GAMEPAD_THUMBSTICK_DEADZONE = (DWORD)(0.20f * 0x7FFF);
const DWORD GAMEPAD_TRIGGER_DEADZONE = 30;
const DWORD MAX_CONTROLLERS = 4;

// Bit corresponding to gamepad button in state.Gamepad.wButtons
const DWORD GAMEPAD_DPAD_UP = 0x0001;
const DWORD GAMEPAD_DPAD_DOWN = 0x0002;
const DWORD GAMEPAD_DPAD_LEFT = 0x0004;
const DWORD GAMEPAD_DPAD_RIGHT = 0x0008;
const DWORD GAMEPAD_START_BUTTON = 0x0010;
const DWORD GAMEPAD_BACK_BUTTON = 0x0020;
const DWORD GAMEPAD_LEFT_THUMB = 0x0040;
const DWORD GAMEPAD_RIGHT_THUMB = 0x0080;
const DWORD GAMEPAD_LEFT_SHOULDER = 0x0100;
const DWORD GAMEPAD_RIGHT_SHOULDER = 0x0200;
const DWORD GAMEPAD_A = 0x1000;
const DWORD GAMEPAD_B = 0x2000;
const DWORD GAMEPAD_X = 0x4000;
const DWORD GAMEPAD_Y = 0x8000;

struct ControllerState
{
    XINPUT_STATE state;
    XINPUT_VIBRATION vibration;
    float vibrateTimeLeft;
    float vibrateTimeRight;
    bool connected;
};

class Input
{
  private:
    bool keysDown[inputNS::KEYS_ARRAY_LEN];       // true if specified key is down
    bool keysPressed[inputNS::KEYS_ARRAY_LEN];    // true if specified key was pressed
    std::string textIn;                           // user entered text
    char charIn;                                  // last character entered
    bool newLine;                                 // true on start of new line
    int mouseX, mouseY;                           // mouse screen coordinates
    int mouseRawX, mouseRawY;                     // high-definition mouse data
    RAWINPUTDEVICE Rid[1];                        // for high-definition mouse
    bool mouseCaptured;                           // true if mouse captured
    bool mouseLButton = false;                    // true if left mouse button down
    bool mouseMButton = false;                    // true if middle mouse button down
    bool mouseRButton = false;                    // true if right mouse button down
    bool mouseX1Button = false;                   // true if X1 mouse button down
    bool mouseX2Button = false;                   // true if X2 mouse button down
    ControllerState controllers[MAX_CONTROLLERS]; // state of controllers

    const ControllerState *getControllerState(UINT n)
    {
        return &controllers[(n > MAX_CONTROLLERS - 1) ? MAX_CONTROLLERS - 1 : n];
    }

  public:
    Input();
    virtual ~Input();
    void initialise(HWND, bool);
    void keyDown(WPARAM);
    void keyUp(WPARAM);
    void keyIn(WPARAM);
    bool isKeyDown(UCHAR) const;
    bool wasKeyPressed(UCHAR) const;
    void clearKeyPress(UCHAR);
    void clear(UCHAR);
    bool anyKeyPressed() const;
    void clearAll() { clear(inputNS::KEYS_MOUSE_TEXT); }
    void cleartextIn() { this->textIn.clear(); }
    std::string getTextIn() { return this->textIn; }
    char getCharIn() { return this->charIn; }

    void mouseIn(LPARAM);
    void mouseRawIn(LPARAM);

    void setMouseLButton(bool b) { this->mouseLButton = b; }
    void setMouseMButton(bool b) { this->mouseMButton = b; }
    void setMouseRButton(bool b) { this->mouseRButton = b; }

    void setMouseXButton(WPARAM wParam)
    {
        this->mouseX1Button = (bool)(wParam & MK_XBUTTON1);
        this->mouseX2Button = (bool)(wParam & MK_XBUTTON2);
    }

    int getMouseX() const { return this->mouseX; }
    int getMouseY() const { return this->mouseY; }

    int getMouseRawX() const { return this->mouseRawX; }
    int getMouseRawY() const { return this->mouseRawY; }

    bool getMouseLButton() const { return mouseLButton; }
    bool getMouseMButton() const { return mouseMButton; }
    bool getMouseRButton() const { return mouseRButton; }
    bool getMouseX1Button() const { return mouseX1Button; }
    bool getMouseX2Button() const { return mouseX2Button; }

    void checkControllers();
    void readControllers();

    const WORD getGamepadButtons(UINT n)
    {
        return (*this->getControllerState(n)).state.Gamepad.wButtons;
    }

    // Match button bit pattern
    bool getGamepadButtonState(UINT n, DWORD bitPattern)
    {
        return (this->getGamepadButtons(n) & bitPattern) != 0;
    }

    BYTE getGamepadLeftTrigger(UINT n)
    {
        return (*this->getControllerState(n)).state.Gamepad.bLeftTrigger;
    }

    BYTE getGamepadRightTrigger(UINT n)
    {
        return (*this->getControllerState(n)).state.Gamepad.bRightTrigger;
    }

    SHORT getGamePadThumbLX(UINT n)
    {
        return (*this->getControllerState(n)).state.Gamepad.sThumbLX;
    }

    SHORT getGamePadThumbLY(UINT n)
    {
        return (*this->getControllerState(n)).state.Gamepad.sThumbLY;
    }

    SHORT getGamePadThumbRX(UINT n)
    {
        return (*this->getControllerState(n)).state.Gamepad.sThumbRX;
    }

    SHORT getGamePadThumbRY(UINT n)
    {
        return (*this->getControllerState(n)).state.Gamepad.sThumbRY;
    }

    // Gamepad vibrate
    // left: low freq, right: high freq
    void gamePadVibrateLeft(UINT n, WORD speed, float sec)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        this->controllers[n].vibration.wLeftMotorSpeed = speed;
        this->controllers[n].vibrateTimeLeft = sec;
    }

    void gamePadVibrateRight(UINT n, WORD speed, float sec)
    {
        if (n > MAX_CONTROLLERS - 1)
            n = MAX_CONTROLLERS - 1;
        this->controllers[n].vibration.wRightMotorSpeed = speed;
        this->controllers[n].vibrateTimeRight = sec;
    }

    void vibrateControllers(float frameTime);
};
