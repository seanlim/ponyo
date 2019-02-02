![](https://res.cloudinary.com/da3pyp8ki/image/upload/v1544777125/ponyo.jpg)

Ponyo is a 2D C++ game engine built on DirectX.

## Core engine

Main engine code resides in `core/`.

## Input

**Gamepad Controller** input is done with `XInput`, bits corresponding to their respective gamepad buttons are mapped, use the getter methods of `Input` to poll for gamepad state.

**Keyboard Input** key polling is done with the default Win32 `GetAsyncKeyState` function. Game commands are mapped with `GameCommand` objects that can be assigned a chord of `KeyBinding`s.

### Key State

| Previous Frame | Current Frame | KeyState     |
| -------------- | ------------- | ------------ |
| 0              | 0             | Released     |
| 0              | 1             | JustPressed  |
| 1              | 0             | JustReleased |
| 1              | 1             | Pressed      |

### Polling for key state

To get the key state in the current frame, use the `getKeyboardKeyState` getter of `Input`.

### Game commands

The input class keeps a map of `GameCommands` to `GameCommand` pointers. A game command can be created like such:

```cpp
// Assign GameCommands in the Input Constructor
keyMap[GameCommands::showFPS] =
      new GameCommand("Toggle FPS", VK_F2, KeyState::JustPressed);
```

### Default game commands

`SHIFT` + `Q` to quit

`F2` to toggle debug info

### Poll for game commands

The `getActiveGameCommands` getter of `Input` returns a vector of currently active GameCommands. From this, actions can be dispatched from a simple loop.

## ECS Support

ECS implementation in `ecs/`.

## Spacewar

Driver game to test engine in `src/`

## TODO

- ECS
- Event system adapter
