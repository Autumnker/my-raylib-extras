# rmouse

Mouse click / double-click / drag detection for [raylib](https://github.com/raysan5/raylib).

## Features

- **Zero-latency click detection** (Windows-style — fires on release, not on a timer)
- **Double-click** with built-in mutual exclusion — `IsClicked()` automatically skips the second click of a double-click pair
- **Drag delta tracking** via `GetDragDelta()` — perfect for pan / move / resize
- **Per-button state machine** — clean, predictable API
- **C and C++ support** — choose `rmouse.hpp` (C++ class) or `rmouse.h` (C struct + functions)

## Usage (C++)

```cpp
#define RMOUSE_IMPLEMENTATION
#include "rmouse.hpp"

int main()
{
    InitWindow(800, 600, "rmouse demo");
    rmouse::Mouse mouse;

    while (!WindowShouldClose())
    {
        mouse.Update();

        // ---- Click / Double-click with mutual exclusion ----
        // IsClicked() automatically skips the second click of a double-click pair.
        if (mouse.IsDoubleClicked(MOUSE_BUTTON_LEFT))
        {
            // Double-click action here
        }
        if (mouse.IsClicked(MOUSE_BUTTON_LEFT))
        {
            // Genuine single-click action here
        }

        // ---- Drag ----
        Vector2 delta = mouse.GetDragDelta(MOUSE_BUTTON_LEFT);
        if (mouse.IsDragging(MOUSE_BUTTON_LEFT))
        {
            // Move / resize using delta
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }
    CloseWindow();
}
```

## Usage (C)

```c
#define RMOUSE_C_IMPLEMENTATION
#include "rmouse.h"

int main()
{
    InitWindow(800, 600, "rmouse C demo");
    RMouse mouse;
    RMouse_Init(&mouse);

    while (!WindowShouldClose())
    {
        RMouse_Update(&mouse);

        if (RMouse_IsDoubleClicked(&mouse, MOUSE_BUTTON_LEFT))
        {
            // Double-click action
        }
        if (RMouse_IsClicked(&mouse, MOUSE_BUTTON_LEFT))
        {
            // Single-click action
        }

        Vector2 delta = RMouse_GetDragDelta(&mouse, MOUSE_BUTTON_LEFT);
        if (RMouse_IsDragging(&mouse, MOUSE_BUTTON_LEFT))
        {
            // Drag action using delta
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }
    CloseWindow();
}
```

> **Note:** The C version currently tracks only the left mouse button. Use separate `RMouse` instances for multiple buttons.

## Dependencies

- [raylib](https://github.com/raysan5/raylib) ≥ 5.5

**Windows (MinGW-w64):** download the prebuilt package from the [releases page](https://github.com/raysan5/raylib/releases#release-5.5) — direct link: [`raylib-5.5_win64_mingw-w64.zip`](https://github.com/raysan5/raylib/releases/download/5.5/raylib-5.5_win64_mingw-w64.zip).

Place raylib next to this repo:

```
your-project/
├── raylib-5.5_win64_mingw-w64/
│   ├── include/
│   └── lib/
└── my-raylib-extras/
    ├── src/
    │   └── rmouse/
    │       ├── rmouse.hpp
    │       ├── rmouse.h
    │       └── README.md          ← you are here
    └── ...
```

## Build

**A) CMake (recommended):**

```sh
# Execute in the root directory (my-raylib-extras/)
cmake -B build -G "MinGW Makefiles" -D RAYLIB_DIR=../raylib-5.5_win64_mingw-w64
cmake --build build
```

**B) Direct compile (C++):**

```sh
# Execute in the root directory (my-raylib-extras/)
g++ -std=c++17 examples/rmouse/example_rmouse.cpp -Isrc/rmouse \
    -I../raylib-5.5_win64_mingw-w64/include \
    -L../raylib-5.5_win64_mingw-w64/lib \
    -lraylib -lopengl32 -lgdi32 -lwinmm -o rmouse_example
```

**C) Direct compile (C):**

```sh
# Execute in the root directory (my-raylib-extras/)
g++ -std=c++17 examples/rmouse/example_rmouse.c -Isrc/rmouse \
    -I../raylib-5.5_win64_mingw-w64/include \
    -L../raylib-5.5_win64_mingw-w64/lib \
    -lraylib -lopengl32 -lgdi32 -lwinmm -o rmouse_example
```

## API (C++)

| Method | Description |
|--------|-------------|
| `Update()` | Call once per frame |
| `IsClicked(button)` | True on the frame a valid click is released |
| `IsDoubleClicked(button)` | True on the frame the second click of a double-click is released |
| `GetDragDelta(button)` | Pixel delta since last frame while dragging |
| `IsDragging(button)` | True while dragging beyond threshold |
| `IsDown(button)` | True while button is physically held |

## API (C)

| Function | Description |
|----------|-------------|
| `RMouse_Init(mouse)` | Initialise an RMouse struct |
| `RMouse_Update(mouse)` | Call once per frame |
| `RMouse_IsClicked(mouse, button)` | True on the frame a valid click is released |
| `RMouse_IsDoubleClicked(mouse, button)` | True on the frame the second click of a double-click is released |
| `RMouse_GetDragDelta(mouse, button)` | Pixel delta since last frame while dragging |
| `RMouse_IsDragging(mouse, button)` | True while dragging beyond threshold |
| `RMouse_IsDown(button)` | True while button is physically held |
