/*
 * rmouse v1.0 - Mouse click / double-click / drag detection for raylib (C version)
 *
 * LICENSE: zlib/libpng
 * Copyright (c) 2026 Autumnker
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.

 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef RMOUSE_H
#define RMOUSE_H

#include "raylib.h"
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------
// Tunable constants
// ---------------------------------------------------------------------------

#define RMOUSE_CLICK_POS_TOLERANCE  8.0f   // px
#define RMOUSE_DRAG_START_THRESHOLD 3.0f   // px
#define RMOUSE_CLICK_MAX_DURATION   0.5f  // seconds
#define RMOUSE_DOUBLE_CLICK_WINDOW  0.3f   // seconds

// ---------------------------------------------------------------------------
// Per-button state (internal)
// ---------------------------------------------------------------------------

typedef struct RMouseButton {
    Vector2 pressPos;
    double pressTime;
    bool isDown;
    bool isDragging;

    Vector2 lastDragPos;
    Vector2 dragDelta;

    double lastClickTime;

    bool clickReady;
    bool doubleClickReady;
    bool isDoubleClickPart;
} RMouseButton;

// ---------------------------------------------------------------------------
// rmouse (C API) — tracks left + right buttons simultaneously
// ---------------------------------------------------------------------------

typedef struct RMouse {
    RMouseButton left;
    RMouseButton right;
} RMouse;

/// Initialise an RMouse struct to safe defaults.
void RMouse_Init(RMouse* mouse);

/// Call once per frame to drive the internal state machine.
void RMouse_Update(RMouse* mouse);

/// Single click: true for the entire frame after a valid click release.
/// Automatically returns false for the second click of a double-click pair.
bool RMouse_IsClicked(const RMouse* mouse, MouseButton button);

/// Double click: true for the entire frame after the second release
/// within RMOUSE_DOUBLE_CLICK_WINDOW.
bool RMouse_IsDoubleClicked(const RMouse* mouse, MouseButton button);

/// Drag delta in pixels accumulated this frame. Does NOT reset on read;
/// reset automatically at the start of the next RMouse_Update().
Vector2 RMouse_GetDragDelta(const RMouse* mouse, MouseButton button);

/// True while the button is held and the mouse has moved beyond
/// RMOUSE_DRAG_START_THRESHOLD.
bool RMouse_IsDragging(const RMouse* mouse, MouseButton button);

/// True while the button is physically held (delegates to raylib).
bool RMouse_IsDown(MouseButton button);

#ifdef __cplusplus
}
#endif

#endif // RMOUSE_H

// ===================================================================
// Implementation — compiled only when RMOUSE_C_IMPLEMENTATION is defined
// ===================================================================
#ifdef RMOUSE_C_IMPLEMENTATION

// ------------------------------------------------------------------
// Internal: select left/right button state by MouseButton
// ------------------------------------------------------------------
static RMouseButton* _btn(RMouse* mouse, MouseButton button)
{
    return (button == MOUSE_BUTTON_LEFT) ? &mouse->left : &mouse->right;
}

static const RMouseButton* _btn_c(const RMouse* mouse, MouseButton button)
{
    return (button == MOUSE_BUTTON_LEFT) ? &mouse->left : &mouse->right;
}

// ------------------------------------------------------------------
// Internal: initialise a single button state
// ------------------------------------------------------------------
static void _btn_init(RMouseButton* btn)
{
    btn->pressPos = (Vector2){ 0, 0 };
    btn->pressTime = 0.0;
    btn->isDown = false;
    btn->isDragging = false;
    btn->lastDragPos = (Vector2){ -1, -1 };
    btn->dragDelta = (Vector2){ 0, 0 };
    btn->lastClickTime = 0.0;
    btn->clickReady = false;
    btn->doubleClickReady = false;
    btn->isDoubleClickPart = false;
}

// ------------------------------------------------------------------
// Internal: update a single button's state machine
// ------------------------------------------------------------------
static void _btn_update(RMouseButton* btn, MouseButton button,
    double now, float mx, float my)
{
    // Reset frame-scoped flags
    btn->clickReady = false;
    btn->doubleClickReady = false;
    btn->isDoubleClickPart = false;
    btn->dragDelta = (Vector2){ 0, 0 };

    // 1. Press
    if (IsMouseButtonPressed(button))
    {
        btn->pressPos = (Vector2){ mx, my };
        btn->pressTime = now;
        btn->isDown = true;
        btn->isDragging = false;
        btn->lastDragPos = (Vector2){ mx, my };
        btn->dragDelta = (Vector2){ 0, 0 };
    }

    // 2. Drag
    if (IsMouseButtonDown(button))
    {
        if (btn->lastDragPos.x >= 0)
        {
            btn->dragDelta.x += mx - btn->lastDragPos.x;
            btn->dragDelta.y += my - btn->lastDragPos.y;
        }
        btn->lastDragPos = (Vector2){ mx, my };

        if (!btn->isDragging)
        {
            float dx = fabsf(mx - btn->pressPos.x);
            float dy = fabsf(my - btn->pressPos.y);
            if (dx >= RMOUSE_DRAG_START_THRESHOLD || dy >= RMOUSE_DRAG_START_THRESHOLD)
            {
                btn->isDragging = true;
            }
        }
    }

    // 3. Release
    if (IsMouseButtonReleased(button))
    {
        bool wasDragging = btn->isDragging;
        btn->isDown = false;
        btn->isDragging = false;
        btn->lastDragPos = (Vector2){ -1, -1 };

        // !wasDragging already guarantees dx/dy < RMOUSE_DRAG_START_THRESHOLD,
        // which is smaller than RMOUSE_CLICK_POS_TOLERANCE, so position check
        // is implied.  Only duration needs explicit verification.
        if (!wasDragging)
        {
            double duration = now - btn->pressTime;

            if (duration < RMOUSE_CLICK_MAX_DURATION)
            {
                bool inDoubleWindow =
                    btn->lastClickTime > 0.0 &&
                    now - btn->lastClickTime < RMOUSE_DOUBLE_CLICK_WINDOW;

                if (inDoubleWindow)
                {
                    btn->doubleClickReady = true;
                    btn->clickReady = true;
                    btn->isDoubleClickPart = true;
                    btn->lastClickTime = 0.0;
                }
                else
                {
                    btn->clickReady = true;
                    btn->isDoubleClickPart = false;
                    btn->lastClickTime = now;
                }
            }
        }
    }
}

// ==================================================================
// Public API
// ==================================================================

void RMouse_Init(RMouse* mouse)
{
    _btn_init(&mouse->left);
    _btn_init(&mouse->right);
}

void RMouse_Update(RMouse* mouse)
{
    double now = GetTime();
    float mx = GetMouseX();
    float my = GetMouseY();

    _btn_update(&mouse->left, MOUSE_BUTTON_LEFT, now, mx, my);
    _btn_update(&mouse->right, MOUSE_BUTTON_RIGHT, now, mx, my);
}

bool RMouse_IsClicked(const RMouse* mouse, MouseButton button)
{
    const RMouseButton* btn = _btn_c(mouse, button);
    if (btn->isDoubleClickPart) return false;
    return btn->clickReady;
}

bool RMouse_IsDoubleClicked(const RMouse* mouse, MouseButton button)
{
    return _btn_c(mouse, button)->doubleClickReady;
}

Vector2 RMouse_GetDragDelta(const RMouse* mouse, MouseButton button)
{
    return _btn_c(mouse, button)->dragDelta;
}

bool RMouse_IsDragging(const RMouse* mouse, MouseButton button)
{
    return _btn_c(mouse, button)->isDragging;
}

bool RMouse_IsDown(MouseButton button)
{
    return IsMouseButtonDown(button);
}

#endif // RMOUSE_C_IMPLEMENTATION
