// Copyright ©2016 Domagoj "oberth" Pandža
// MIT license | Read LICENSE.txt for details.

#pragma once
#include <string>

#include <platform/window.hpp>
#include <graphics/renderer.hpp>
#include <graphics/image.hpp>

namespace chrome {

    struct application {

        application() = delete;
        application(char** args, int argument_count);

        application(application const&) = delete;
        application(application&&) = delete;

        application& operator=(application const&) = delete;
        application& operator=(application&&) = delete;

        ~application() = default;

        auto execute() -> int;
        auto paint_window() -> void;


        auto& get_window_margins() { return window->get_margins(); }
        auto set_focus(bool in_focus) { application_window_in_focus = in_focus; }
        auto on_size_changed() { if (window) renderer->resize_buffer(); }

    private:
        
        auto paint_mock_tabs(graphics::rectangle<float> const& window_rectangle, MARGINS const& margins) -> void;
        auto paint_mock_toolbar(graphics::rectangle<float> const& window_rectangle, MARGINS const& margins) -> void;
        auto paint_mock_sidebar(graphics::rectangle<float> const& window_rectangle, MARGINS const& margins) -> void;
        auto paint_mock_caption() -> void;

        std::unique_ptr<platform::window> window;
        std::unique_ptr<graphics::renderer> renderer;

        std::unique_ptr<resource::image> tab_raster;
        std::unique_ptr<resource::image> windows_logo;
        std::unique_ptr<resource::image> windows_logo_dark;
        std::unique_ptr<resource::image> new_tab_symbol;

        bool application_window_in_focus;

    };

}

