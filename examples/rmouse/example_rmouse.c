/*
 * rmouse C example — demonstrates click, double-click (with mutual exclusion), and drag detection.
 *
 * Build (MinGW-w64 + raylib 5.5):
 * # Execute in the root directory (my-raylib-extras/)
 *
 * gcc -std=c11 examples/rmouse/example_rmouse.c -Isrc/rmouse \
 *     -I../raylib-5.5_win64_mingw-w64/include \
 *     -L../raylib-5.5_win64_mingw-w64/lib \
 *     -lraylib -lopengl32 -lgdi32 -lwinmm -o rmouse_c_example
 */

#define RMOUSE_C_IMPLEMENTATION
#include "rmouse.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// ------------------------------------------------------------------
// Helper: draw a simple text log on screen (C-style, fixed-size ring buffer)
// ------------------------------------------------------------------
#define MAX_LOG_LINES 20
#define MAX_LOG_LEN  128

typedef struct {
    char text[MAX_LOG_LEN];
    float lifetime;
} LogLine;

static LogLine gLog[MAX_LOG_LINES];
static int gLogCount = 0;

static void AddLog(const char* fmt, ...)
{
    if (gLogCount >= MAX_LOG_LINES) return;
    char buf[MAX_LOG_LEN];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    strcpy(gLog[gLogCount].text, buf);
    gLog[gLogCount].lifetime = 2.5f;
    gLogCount++;
}

static void UpdateLog(float dt)
{
    int writeIdx = 0;
    for (int i = 0; i < gLogCount; i++)
    {
        if (gLog[i].lifetime > 0.0f)
        {
            gLog[i].lifetime -= dt;
            if (writeIdx != i)
                gLog[writeIdx] = gLog[i];
            writeIdx++;
        }
    }
    gLogCount = writeIdx;
}

static void DrawLog(void)
{
    int y = 10;
    for (int i = 0; i < gLogCount; i++)
    {
        DrawText(gLog[i].text, 10, y, 18, BLACK);
        y += 20;
    }
}

// ------------------------------------------------------------------
// Main
// ------------------------------------------------------------------
int main(void)
{
    InitWindow(800, 600, "rmouse C example");
    SetTargetFPS(60);

    RMouse mouse;
    RMouse_Init(&mouse);

    // Draggable rectangle
    Rectangle rect = { 300, 250, 200, 100 };
    bool draggingRect = false;

    // Click / double-click state for the "undo on double-click" pattern
    Color rectColor = BLUE;
    Color prevRectColor = BLUE;

    while (!WindowShouldClose())
    {
        RMouse_Update(&mouse);

        // Tick log lifetimes
        float dt = GetFrameTime();
        UpdateLog(dt);

        Vector2 mp = GetMousePosition();
        bool hoverRect = CheckCollisionPointRec(mp, rect);

        // ── Double-click on rect -> cycle colour ──
        if (RMouse_IsDoubleClicked(&mouse, MOUSE_BUTTON_LEFT) && hoverRect)
        {
            // Undo the colour change caused by the first click of the pair
            rectColor = prevRectColor;

            // Double-click action: cycle to next colour
            Color colors[] = { BLUE, RED, GREEN, ORANGE, PURPLE, MAROON };
            const char* colorNames[] = { "Blue", "Red", "Green", "Orange", "Purple", "Maroon" };
            static int ci = 0;
            ci = (ci + 1) % 6;
            rectColor = colors[ci];
            AddLog("DOUBLE-CLICK on rect  -> colour # %s", colorNames[ci]);
        }

        // ── Click handling ──
        if (RMouse_IsClicked(&mouse, MOUSE_BUTTON_LEFT) && hoverRect)
        {
            // Single click on rect -> darken
            prevRectColor = rectColor;
            rectColor = (Color){
                (unsigned char)(rectColor.r / 2),
                (unsigned char)(rectColor.g / 2),
                (unsigned char)(rectColor.b / 2),
                255
            };
            AddLog("CLICK on rect -> darkened");
        }
        if (RMouse_IsClicked(&mouse, MOUSE_BUTTON_LEFT) && !hoverRect)
        {
            // Click on background -> reset
            prevRectColor = rectColor;
            rectColor = BLUE;
            AddLog("CLICK on background -> reset colour");
        }

        // ── Drag the rectangle ──
        if (RMouse_IsDragging(&mouse, MOUSE_BUTTON_LEFT) && hoverRect && !draggingRect)
        {
            draggingRect = true;
        }
        if (draggingRect)
        {
            if (RMouse_IsDragging(&mouse, MOUSE_BUTTON_LEFT))
            {
                Vector2 d = RMouse_GetDragDelta(&mouse, MOUSE_BUTTON_LEFT);
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
        if (RMouse_IsClicked(&mouse, MOUSE_BUTTON_RIGHT))
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
