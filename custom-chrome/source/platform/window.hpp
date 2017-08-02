// Copyright ©2016 Domagoj "oberth" Pandža
// MIT license | Read LICENSE.txt for details.

#pragma once
#include <string>
#include <Windows.h>
#include <dwmapi.h>

namespace platform {

    using window_event_handler = LRESULT(CALLBACK*)(HWND window_handle, UINT message, WPARAM wparam, LPARAM);

    struct window {

        window() = delete;

        window(
            std::wstring const& title, std::uint32_t const width, std::uint32_t const height,
            std::uint32_t const caption_height, void* application_ptr, window_event_handler event_handler
        );

        window(window const&) = delete; // Cannot be copied.
        window(window&&) = default; // Default move will do.

        window& operator=(window const&) = delete;
        window& operator=(window&&) = default;

        ~window();

        auto show_window() -> void;
        auto get_system_window_handle() const { return system_window_handle; }
        auto& get_margins() const { return margins; }
        auto& get_title() const { return title; }

        auto get_client_area() {
            RECT client_rectangle; GetClientRect(system_window_handle, &client_rectangle);
            auto p2l_scaling_factor = 1.0f / user_scaling;

            return RECT {
                static_cast<int>(std::ceilf(p2l_scaling_factor * client_rectangle.left)),
                static_cast<int>(std::ceilf(p2l_scaling_factor * client_rectangle.top)),
                static_cast<int>(std::ceilf(p2l_scaling_factor * client_rectangle.right)),
                static_cast<int>(std::ceilf(p2l_scaling_factor * client_rectangle.bottom)),
            };
        }

    private:
        HWND system_window_handle;
        std::wstring title;
        MARGINS margins;
        float user_scaling;
    };

}