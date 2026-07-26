/*
 * rmouse example — demonstrates click, double-click (with mutual exclusion), and drag detection.
 *
 * Build (MinGW-w64 + raylib 5.5):
 *   g++ -std=c++17 main.cpp -Isrc -I../../raylib-5.5_win64_mingw-w64/include \
 *       -L../../raylib-5.5_win64_mingw-w64/lib -lraylib -lopengl32 -lgdi32 -lwinmm -o example_demo
 */

#define RMOUSE_IMPLEMENTATION
#include "../src/rmouse.h"
#include <cstdarg>
#include <cstdio>
#include <string>
#include <list>

// ------------------------------------------------------------------
// Helper: draw a simple text log on screen
// ------------------------------------------------------------------
struct LogLine {
    std::string text;
    float lifetime = 0.0f;  // seconds remaining
};

static std::list<LogLine> g_log;

static void AddLog(const char* fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    std::vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    g_log.push_back(LogLine{ buf,2.5f });
}

static void DrawLog() {
    int y = 10;
    for (auto& log : g_log) {
        if (log.lifetime > 0.0f) {
            DrawText(log.text.c_str(), 10, y, 18, BLACK);
            y += 20;
        }
    }
}

// ------------------------------------------------------------------
// Main
// ------------------------------------------------------------------
int main() {
    InitWindow(800, 600, "rmouse example");
    SetTargetFPS(60);

    rmouse::Mouse mouse;

    // Draggable rectangle
    Rectangle rect = { 300, 250, 200, 100 };
    bool dragging_rect = false;

    // Click / double-click state for the "undo on double-click" pattern
    Color rect_color = BLUE;
    Color prev_rect_color = BLUE;

    while (!WindowShouldClose()) {
        mouse.Update();

        // Tick log lifetimes
        float dt = GetFrameTime();
        for (auto it = g_log.begin(); it != g_log.end(); it++) {
            if ((*it).lifetime > 0) {
                (*it).lifetime -= dt;
            }
            else {
                g_log.erase(it);
            }
        }

        Vector2 mp = GetMousePosition();
        bool hover_rect = CheckCollisionPointRec(mp, rect);

        // ── Double-click on rect -> cycle colour ──
        if (mouse.IsDoubleClicked(MOUSE_BUTTON_LEFT) && hover_rect) {
            // Undo the colour change caused by the first click of the pair
            rect_color = prev_rect_color;

            // Double-click action: cycle to next colour
            Color colors[] = { BLUE, RED, GREEN, ORANGE, PURPLE, MAROON };
            std::string colors_name[] = { "Blue", "Red", "Green", "Orange", "Purple", "Maroon" };
            static int ci = 0;
            ci = (ci + 1) % 6;
            rect_color = colors[ci];
            AddLog("DOUBLE-CLICK on rect  -> colour # %s", colors_name[ci].c_str());
        }

        // ── Click handling ──
        if (mouse.IsClicked(MOUSE_BUTTON_LEFT) && hover_rect) {
            // Single click on rect -> darken
            prev_rect_color = rect_color;
            rect_color = Color{
                (unsigned char)(rect_color.r / 2),
                (unsigned char)(rect_color.g / 2),
                (unsigned char)(rect_color.b / 2),
                255
            };
            AddLog("CLICK on rect -> darkened");
        }
        if (mouse.IsClicked(MOUSE_BUTTON_LEFT) && !hover_rect) {
            // Click on background -> reset
            prev_rect_color = rect_color;
            rect_color = BLUE;
            AddLog("CLICK on background -> reset colour");
        }

        // ── Drag the rectangle ──
        if (mouse.IsDragging(MOUSE_BUTTON_LEFT) && hover_rect && !dragging_rect) {
            dragging_rect = true;
        }
        if (dragging_rect) {
            if (mouse.IsDragging(MOUSE_BUTTON_LEFT)) {
                Vector2 d = mouse.GetDragDelta(MOUSE_BUTTON_LEFT);
                rect.x += d.x;
                rect.y += d.y;
            } else {
                dragging_rect = false;
                AddLog("Drag ended");
            }
        }

        // ── Right-click anywhere ──
        if (mouse.IsClicked(MOUSE_BUTTON_RIGHT)) {
            AddLog("RIGHT CLICK");
        }

        // ── Draw ──
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleRec(rect, rect_color);
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
