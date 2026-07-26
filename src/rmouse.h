/*
 * rmouse v1.0 - Mouse click / double-click / drag detection for raylib
 *
 * LICENSE: zlib/libpng
 * Copyright (c) 2026 Autumnker
 */

#ifndef RMOUSE_H
#define RMOUSE_H

#include "raylib.h"
#include <cmath>

// ---------------------------------------------------------------------------
// Tunable constants
// ---------------------------------------------------------------------------

/// Max distance between press and release positions for a valid click.
/// Exceeding this is treated as a drag instead.
constexpr float CLICK_POS_TOLERANCE = 8.0f;

/// Minimum distance the mouse must move after pressing to be considered
/// a drag (avoids jitter).
constexpr float DRAG_START_THRESHOLD = 3.0f;

/// Maximum duration of a press for it to qualify as a click (seconds).
/// Longer presses are ignored.
constexpr float CLICK_MAX_DURATION = 0.5f;  // 500 ms

/// Time window between two consecutive clicks for them to form a
/// double-click (seconds).
constexpr float DOUBLE_CLICK_WINDOW = 0.3f;  // 300 ms

// ---------------------------------------------------------------------------
// rmouse::Mouse
// ---------------------------------------------------------------------------

namespace rmouse {

/**
 * @brief Zero-latency mouse click / double-click / drag state machine
 *        (Windows-style: IsClicked fires immediately on release).
 *
 * Key idea — mimics Windows WM_LBUTTONDOWN / WM_LBUTTONDBLCLK:
 *   - On every valid click release, IsClicked() returns true instantly.
 *   - If a second click happens within DOUBLE_CLICK_WINDOW,
 *     IsDoubleClicked() returns true on that release.
 *   - IsClicked() automatically suppresses the second click of a
 *     double-click pair.
 *
 * Usage example (click / double-click mutual exclusion):
 *
 *   static int  value = 0;
 *   static int  prev_value = 0;
 *
 *   if (mouse.IsDoubleClicked(MOUSE_BUTTON_LEFT)) {
 *       value = prev_value;    // undo the first click's change
 *       value += 10;       // double-click action
 *   }
 *   if (mouse.IsClicked(MOUSE_BUTTON_LEFT)) {
 *       prev_value = value;   // save before changing
 *       value += 1;       // single-click action
 *   }
 */
class Mouse {
public:
    Mouse() = default;
    virtual ~Mouse() = default;

    /// Call once per frame to drive the internal state machine.
    void Update();

    // ---- Frame-scoped queries (same result for the whole frame) ----

    /// Single click: true for the entire frame after a valid click release.
    /// Automatically returns false for the second click of a double-click pair.
    bool IsClicked(MouseButton button) const;
    /// Double click: true for the entire frame after the second release
    /// within DOUBLE_CLICK_WINDOW.
    bool IsDoubleClicked(MouseButton button) const;
    /// Drag delta in pixels accumulated this frame.  Does NOT reset on read;
    /// reset automatically at the start of the next Update().
    Vector2 GetDragDelta(MouseButton button) const;

    // ---- Stateful interfaces (reflect persistent per-frame state) ----

    /// True while the button is held and the mouse has moved beyond
    /// DRAG_START_THRESHOLD.
    bool IsDragging(MouseButton button) const;
    /// True while the button is physically held (delegates to raylib).
    bool IsDown(MouseButton button) const;

private:
    struct ButtonState {
        // Current press info
        Vector2 press_pos_{ 0, 0 };
        double press_time_ = 0.0;  // GetTime() timestamp
        bool is_down_ = false;
        bool is_dragging_ = false;

        // Drag tracking
        Vector2 last_drag_pos_{ -1, -1 };
        Vector2 drag_delta_{ 0, 0 };

        // Release time of the previous complete click (for double-click detection)
        double last_click_time_ = 0.0;  // GetTime() timestamp

        // Frame-scoped event flags (reset at the start of each Update())
        bool click_ready_ = false;
        bool double_click_ready_ = false;
        bool is_double_click_part_ = false;  // click_ready_ is the 2nd half of a double-click
    };

    ButtonState left_;
    ButtonState right_;

    void UpdateButton_(ButtonState& s, MouseButton button);
};

} // namespace rmouse

#endif // RMOUSE_H

// ===================================================================
// Implementation — compiled only when RMOUSE_IMPLEMENTATION is defined
// ===================================================================
#ifdef RMOUSE_IMPLEMENTATION

namespace rmouse {

// ------------------------------------------------------------------
// Public API
// ------------------------------------------------------------------

void Mouse::Update() {
    // Reset all frame-scoped flags from the previous frame so that
    // IsClicked(), IsDoubleClicked(), and GetDragDelta() can be called
    // any number of times within the same frame with consistent results.
    left_.click_ready_       = false;
    left_.double_click_ready_   = false;
    left_.is_double_click_part_  = false;
    left_.drag_delta_       = { 0, 0 };
    right_.click_ready_      = false;
    right_.double_click_ready_   = false;
    right_.is_double_click_part_  = false;
    right_.drag_delta_       = { 0, 0 };

    UpdateButton_(left_, MOUSE_BUTTON_LEFT);
    UpdateButton_(right_, MOUSE_BUTTON_RIGHT);
}

bool Mouse::IsClicked(MouseButton button) const {
    const ButtonState& s = (button == MOUSE_BUTTON_LEFT) ? left_ : right_;
    // Suppress the second click of a double-click pair.
    if (s.is_double_click_part_)
        return false;
    return s.click_ready_;
}

bool Mouse::IsDoubleClicked(MouseButton button) const {
    const ButtonState& s = (button == MOUSE_BUTTON_LEFT) ? left_ : right_;
    return s.double_click_ready_;
}

bool Mouse::IsDragging(MouseButton button) const {
    const ButtonState& s = (button == MOUSE_BUTTON_LEFT) ? left_ : right_;
    return s.is_dragging_;
}

bool Mouse::IsDown(MouseButton button) const {
    return ::IsMouseButtonDown(button);
}

Vector2 Mouse::GetDragDelta(MouseButton button) const {
    const ButtonState& s = (button == MOUSE_BUTTON_LEFT) ? left_ : right_;
    return s.drag_delta_;
}

// ------------------------------------------------------------------
// Per-button state machine (Windows-style: zero-latency click)
// ------------------------------------------------------------------

void Mouse::UpdateButton_(ButtonState& s, MouseButton button) {
    double now = GetTime();
    float mx = ::GetMousePosition().x;
    float my = ::GetMousePosition().y;

    // ── 1. Press: record initial state ──
    if (::IsMouseButtonPressed(button)) {
        s.press_pos_ = { mx, my };
        s.press_time_ = now;
        s.is_down_ = true;
        s.is_dragging_ = false;
        s.last_drag_pos_ = { mx, my };
        s.drag_delta_ = { 0, 0 };
    }

    // ── 2. Held: accumulate drag delta, detect drag start ──
    if (::IsMouseButtonDown(button)) {
        if (s.last_drag_pos_.x >= 0) {
            s.drag_delta_.x += mx - s.last_drag_pos_.x;
            s.drag_delta_.y += my - s.last_drag_pos_.y;
        }
        s.last_drag_pos_ = { mx, my };

        if (!s.is_dragging_) {
            float dx = std::abs(mx - s.press_pos_.x);
            float dy = std::abs(my - s.press_pos_.y);
            if (dx >= DRAG_START_THRESHOLD || dy >= DRAG_START_THRESHOLD) {
                s.is_dragging_ = true;
            }
        }
    }

    // ── 3. Release: classify as click / double-click ──
    if (::IsMouseButtonReleased(button)) {
        s.is_down_     = false;
        s.is_dragging_ = false;
        s.last_drag_pos_ = { -1, -1 };

        if (!s.is_dragging_) {
            float dx = std::abs(mx - s.press_pos_.x);
            float dy = std::abs(my - s.press_pos_.y);
            double duration = now - s.press_time_;

            // Close position + short duration -> valid click
            if (dx < CLICK_POS_TOLERANCE && dy < CLICK_POS_TOLERANCE &&
                duration < CLICK_MAX_DURATION) {

                // Check whether we are within the double-click window
                bool in_double_window =
                    s.last_click_time_ > 0.0 &&
                    now - s.last_click_time_ < DOUBLE_CLICK_WINDOW;

                if (in_double_window) {
                    // Double click!  Also mark this click as the second half.
                    s.double_click_ready_ = true;
                    s.click_ready_ = true;
                    s.is_double_click_part_ = true;
                    s.last_click_time_ = 0.0;  // Reset to avoid triple-click false positive
                }
                else {
                    // Standalone click (may be the first of a pair, or a true single).
                    // Fire immediately either way.
                    s.click_ready_ = true;
                    s.is_double_click_part_ = false;
                    s.last_click_time_ = now;
                }
            }
        }
        // Drag release: no click event is produced
    }
}

} // namespace rmouse

#endif // RMOUSE_IMPLEMENTATION
