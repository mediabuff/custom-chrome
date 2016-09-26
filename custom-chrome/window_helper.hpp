// Copyright ©2016 Domagoj "oberth" Pandža
// MIT license | Read LICENSE.txt for details.

#pragma once

#include <Windows.h>
#include <windowsx.h>

#include "common.hpp"

// Simplest variant, just to get you started.
LRESULT compute_sector_of_window(HWND window_handle, WPARAM wparam, LPARAM lparam, int caption_height) {

    // Acquire the window rect
    RECT window_rectangle;
    GetWindowRect(window_handle, &window_rectangle);

    auto offset = 10;

    POINT cursor_position{
        GET_X_LPARAM(lparam),
        GET_Y_LPARAM(lparam)
    };

    if (cursor_position.y < window_rectangle.top + offset && cursor_position.x < window_rectangle.left + offset) return HTTOPLEFT;
    if (cursor_position.y < window_rectangle.top + offset && cursor_position.x > window_rectangle.right - offset) return HTTOPRIGHT;
    if (cursor_position.y > window_rectangle.bottom - offset && cursor_position.x > window_rectangle.right - offset) return HTBOTTOMRIGHT;
    if (cursor_position.y > window_rectangle.bottom - offset && cursor_position.x < window_rectangle.left + offset) return HTBOTTOMLEFT;

    if (cursor_position.x > window_rectangle.left && cursor_position.x < window_rectangle.right) {
        if (cursor_position.y < window_rectangle.top + offset) return HTTOP;
        else if (cursor_position.y > window_rectangle.bottom - offset) return HTBOTTOM;
    }
    if (cursor_position.y > window_rectangle.top && cursor_position.y < window_rectangle.bottom) {
        if (cursor_position.x < window_rectangle.left + offset) return HTLEFT;
        else if (cursor_position.x > window_rectangle.right - offset) return HTRIGHT;
    }

    if (cursor_position.x > window_rectangle.left && cursor_position.x < window_rectangle.right) {
        if (cursor_position.y < window_rectangle.top + caption_height) return HTCAPTION;
    }

    return HTNOWHERE;

}