# rmouse

Mouse click / double-click / drag detection for [raylib](https://github.com/raysan5/raylib).

## Features

- **Zero-latency click detection** (Windows-style — fires on release, not on a timer)
- **Double-click** with built-in mutual exclusion — `IsClicked()` automatically skips the second click of a double-click pair
- **Drag delta tracking** via `GetDragDelta()` — perfect for pan / move / resize
- **Per-button state machine** — clean, predictable API
- **Single-header** — `#define RMOUSE_IMPLEMENTATION` and you're done

## Usage

```cpp
#define RMOUSE_IMPLEMENTATION
#include "rmouse.h"

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

> **Note:** `rmouse` is C++ only (uses namespaces, classes). You also need to link against raylib.

## Dependencies

- [raylib](https://github.com/raysan5/raylib) ≥ 5.5

**Windows (MinGW-w64):** download the prebuilt package from the [releases page](https://github.com/raysan5/raylib/releases#release-5.5) — direct link: [`raylib-5.5_win64_mingw-w64.zip`](https://github.com/raysan5/raylib/releases/download/5.5/raylib-5.5_win64_mingw-w64.zip).

Place raylib next to this repo:

```
your-project/
├── raylib-5.5_win64_mingw-w64/   ← NOT in this repo
│   ├── include/
│   └── lib/
└── my-raylib-extras/
    ├── src/
    │   └── rmouse/
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

**B) Direct compile:**

```sh
# Execute in the root directory (my-raylib-extras/)
g++ -std=c++17 examples/rmouse/main.cpp -Isrc/rmouse \
    -I../raylib-5.5_win64_mingw-w64/include \
    -L../raylib-5.5_win64_mingw-w64/lib \
    -lraylib -lopengl32 -lgdi32 -lwinmm -o rmouse_example
```

## API

| Method | Description |
|--------|-------------|
| `Update()` | Call once per frame; resets all frame-scoped flags, then runs the state machine |
| `IsClicked(button)` | True for the entire frame after a valid click release; skips the 2nd click of a double-click pair. **Not consuming** — call it anywhere, any number of times |
| `IsDoubleClicked(button)` | True for the entire frame on the second click of a double-click pair. **Not consuming** |
| `GetDragDelta(button)` | Frame drag offset in pixels; **not consuming** — reset automatically by `Update()` |
| `IsDragging(button)` | True while dragging (mouse moved past threshold) |
| `IsDown(button)` | True while button is held (delegates to raylib) |

## Constants

Adjust these before `#include` if needed:

```cpp
#define CLICK_POS_TOLERANCE   8.0f   // max press -> release distance for a click
#define DRAG_START_THRESHOLD  3.0f   // min movement to start a drag
#define CLICK_MAX_DURATION    0.5f   // max press duration (seconds)
#define DOUBLE_CLICK_WINDOW   0.3f   // max gap between clicks (seconds)
```

## Example

See [`examples/rmouse/main.cpp`](../../examples/rmouse/main.cpp) for a runnable demo.

## License

zlib/libpng — same as raylib.
