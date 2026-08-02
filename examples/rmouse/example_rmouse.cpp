/*
 * rmouse example — demonstrates click, double-click (with mutual exclusion), and drag detection.
 *
 * Build (MinGW-w64 + raylib 5.5):
 * # Execute in the root directory (my-raylib-extras/)
 * 
 * g++ -std=c++17 examples/rmouse/example_rmouse.cpp -Isrc/rmouse \
 *     -I../raylib-5.5_win64_mingw-w64/include \
 *     -L../raylib-5.5_win64_mingw-w64/lib \
 *     -lraylib -lopengl32 -lgdi32 -lwinmm -o rmouse_example
 */

#define RMOUSE_IMPLEMENTATION
#include "rmouse.hpp"
#include <cstdarg>
#include <cstdio>
#include <string>
#include <list>

// ------------------------------------------------------------------
// Helper: draw a simple text log on screen
// ------------------------------------------------------------------
struct LogLine
{
    std::string text;
    float lifetime = 0.0f;  // seconds remaining
};

static std::list<LogLine> gLog;

static void AddLog(const char* fmt, ...)
{
    char buf[256];
    va_list args;
    va_start(args, fmt);
    std::vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    gLog.push_back(LogLine{ buf,2.5f });
}

static void DrawLog()
{
    int y = 10;
    for (auto& log : gLog)
    {
        if (log.lifetime > 0.0f)
        {
            DrawText(log.text.c_str(), 10, y, 18, BLACK);
            y += 20;
        }
    }
}

// ------------------------------------------------------------------
// Main
// ------------------------------------------------------------------
int main()
{
    InitWindow(800, 600, "rmouse example");
    SetTargetFPS(60);

    rmouse::Mouse mouse;

    // Draggable rectangle
    Rectangle rect = { 300, 250, 200, 100 };
    bool draggingRect = false;

    // Click / double-click state for the "undo on double-click" pattern
    Color rectColor = BLUE;
    Color prevRectColor = BLUE;

    while (!WindowShouldClose())
    {
        mouse.Update();

        // Tick log lifetimes
        float dt = GetFrameTime();
        for (auto it = gLog.begin(); it != gLog.end();)
        {
            if ((*it).lifetime > 0)
            {
                (*it).lifetime -= dt;
                ++it;
            }
            else
            {
                it = gLog.erase(it);
            }
        }

        Vector2 mp = GetMousePosition();
        bool hoverRect = CheckCollisionPointRec(mp, rect);

        // ── Double-click on rect -> cycle colour ──
        if (mouse.IsDoubleClicked(MOUSE_BUTTON_LEFT) && hoverRect)
        {
            // Undo the colour change caused by the first click of the pair
            rectColor = prevRectColor;

            // Double-click action: cycle to next colour
            Color colors[] = { BLUE, RED, GREEN, ORANGE, PURPLE, MAROON };
            std::string colorsName[] = { "Blue", "Red", "Green", "Orange", "Purple", "Maroon" };
            static int ci = 0;
            ci = (ci + 1) % 6;
            rectColor = colors[ci];
            AddLog("DOUBLE-CLICK on rect  -> colour # %s", colorsName[ci].c_str());
        }

        // ── Click handling ──
        if (mouse.IsClicked(MOUSE_BUTTON_LEFT) && hoverRect)
        {
            // Single click on rect -> darken
            prevRectColor = rectColor;
            rectColor = Color{
                (unsigned char)(rectColor.r / 2),
                (unsigned char)(rectColor.g / 2),
                (unsigned char)(rectColor.b / 2),
                255
            };
            AddLog("CLICK on rect -> darkened");
        }
        if (mouse.IsClicked(MOUSE_BUTTON_LEFT) && !hoverRect)
        {
            // Click on background -> reset
            prevRectColor = rectColor;
            rectColor = BLUE;
            AddLog("CLICK on background -> reset colour");
        }

        // ── Drag the rectangle ──
        if (mouse.IsDragging(MOUSE_BUTTON_LEFT) && hoverRect && !draggingRect)
        {
            draggingRect = true;
        }
        if (draggingRect)
        {
            if (mouse.IsDragging(MOUSE_BUTTON_LEFT))
            {
                Vector2 d = mouse.GetDragDelta(MOUSE_BUTTON_LEFT);
                rect.x += d.x;
                rect.y += d.y;
            }
            else
            {
                draggingRect = false;
                AddLog("Drag ended");
            }
        }

        // ── Right-click anywhere ──
        if (mouse.IsClicked(MOUSE_BUTTON_RIGHT))
        {
            AddLog("RIGHT CLICK");
        }

        // ── Draw ──
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleRec(rect, rectColor);
        DrawRectangleLinesEx(rect, 2, DARKGRAY);
        DrawText("Drag me / click me", (int)rect.x + 5, (int)rect.y + 40, 24, WHITE);

        DrawText("Left: click/double-click/drag  |  Right: click", 10, 570, 20, LIGHTGRAY);
        DrawLog();
        DrawFPS(700, 560);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
