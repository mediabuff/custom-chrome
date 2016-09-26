// Copyright ©2016 Domagoj "oberth" Pandža
// MIT license | Read LICENSE.txt for details.

#pragma once
#include <string>
#include <Windows.h>
#include <dwmapi.h>

#include "common.hpp"

namespace platform {

    using window_event_handler = LRESULT(CALLBACK*)(HWND window_handle, UINT message, WPARAM wparam, LPARAM);

    class window {
    public:

        window() = delete;

        window(
            std::wstring const& title, std::uint32_t const width, std::uint32_t const height,
            std::uint32_t const caption_height, std::uint32_t const border_width, 
            void* application_ptr, window_event_handler event_handler
        );

        window(window const&) = delete; // Cannot be copied.
        window(window&&) = default; // Default move will do.

        window& operator=(window const&) = delete;
        window& operator=(window&&) = default;

        ~window() = default;

        auto show_window() -> void;
        auto get_system_window_handle() const { return system_window_handle; }
        auto& get_margins() const { return margins; }
        auto& get_title() const { return title; }

    private:
        HWND system_window_handle;
        std::wstring title;
        MARGINS margins;
    };

}