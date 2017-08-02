#include <array>
#include <sstream>
#include <dwmapi.h>

#include <application.hpp>
#include <platform/window_helper.hpp>

auto CALLBACK process_message(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam) -> LRESULT {

    LRESULT lr = 0;
    // Ask whether DWM would like to process the incoming message (to handle caption butans.
    auto dwm_processed = DwmDefWindowProc(window_handle, message, wparam, lparam, &lr);
    auto application = reinterpret_cast<chrome::application*>(GetWindowLongPtr(window_handle, GWLP_USERDATA));

    if (message == WM_CREATE) {
        auto create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
        SetWindowLongPtrW(window_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create_struct->lpCreateParams));

        // We need to trigger recompute of the window and client area.
        SetWindowPos(window_handle, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
    }

    else if (message == WM_ACTIVATE) {
        bool in_focus = static_cast<bool>(LOWORD(wparam));
        application->set_focus(in_focus);
        InvalidateRect(window_handle, nullptr, true);
    }

    // Extends the client area all around (returning 0 when wparam is TRUE)
    else if (message == WM_NCCALCSIZE) {

        auto client_area_needs_calculating = static_cast<bool>(wparam);

        if(client_area_needs_calculating) {
            auto parameters = reinterpret_cast<NCCALCSIZE_PARAMS*>(lparam);

            auto& requested_client_area = parameters->rgrc[0];
            requested_client_area.right -= GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
            requested_client_area.left += GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
            requested_client_area.bottom -= GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);

            return 0;
        }

    }

    // Determine whether the cursor is near interactive points of the window
    else if ((message == WM_NCHITTEST) && (lr == 0)) {
        auto dpi = GetDpiForSystem();
        auto user_scaling = static_cast<float>(dpi) / 96.0f;
        auto caption_height = application->get_window_margins().cyTopHeight;
        auto corrected_height = static_cast<int>(std::ceilf(user_scaling * caption_height));
        lr = compute_sector_of_window(window_handle, wparam, lparam, corrected_height);
        if (lr != HTNOWHERE) return lr;
    }

    else if (message == WM_PAINT) {
        application->paint_window();
    }

    else if (message == WM_SIZE) {
        application->on_size_changed();
    }

    else if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    if (!dwm_processed) return DefWindowProcW(window_handle, message, wparam, lparam);
    else return lr;

}

namespace chrome {

    application::application(char**, int) {

        try {

            tab_raster = std::make_unique<resource::image>(L"media/tab_raster.png");
            windows_logo = std::make_unique<resource::image>(L"media/winlogo.png");
            windows_logo_dark = std::make_unique<resource::image>(L"media/winlogo_dark.png");
            new_tab_symbol = std::make_unique<resource::image>(L"media/new_tab_symbol.png");

            renderer = std::make_unique<graphics::renderer>();
            auto caption_area_expansion_height = 80; // A value lower than 75 will remove the title

            window = std::make_unique<platform::window>(
                L"Chrome management", 1200, 600, 
                caption_area_expansion_height, this, 
                process_message
            );

            renderer->attach_to_window(window->get_system_window_handle());
            window->show_window(); // Crucial separation that the window is valid when all the messages start flying.

        }

        catch (std::exception const& exception) {
            std::wstringstream exception_message;
            exception_message << exception.what();
            MessageBoxW(nullptr, exception_message.str().c_str(), L"Fatal error encountered.", MB_ICONERROR | MB_OK);
            std::terminate();
        }

    }

    auto application::execute() -> int {

        MSG message_structure{};

        while (message_structure.message != WM_QUIT)
        if (PeekMessageW(&message_structure, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message_structure);
            DispatchMessageW(&message_structure);
        }

        return static_cast<int>(message_structure.wParam);

    }

    auto application::paint_window() -> void {

        renderer->begin_draw();

        auto client_rectangle = window->get_client_area(); 
        auto margins = window->get_margins();

        graphics::rectangle<float> client_area {
            static_cast<float>(client_rectangle.left),
            static_cast<float>(client_rectangle.top + margins.cyTopHeight),
            static_cast<float>(client_rectangle.right),
            static_cast<float>(client_rectangle.bottom)
        };

        // Paint the whole client area into our baseline color.
        renderer->fill_rectangle(
            graphics::rectangle<float> { 
                client_area.left,
                client_area.top,
                client_area.right,
                client_area.bottom
            }, graphics::color{ 0.96f, 0.96f, 0.96f }
        );

        if(margins.cyTopHeight > 74.0f) paint_mock_caption();
        paint_mock_tabs(client_area, margins);
        paint_mock_toolbar(client_area, margins);
        paint_mock_sidebar(client_area, margins);


        renderer->end_draw();

    }

    auto application::paint_mock_tabs(graphics::rectangle<float> const& window_rectangle, MARGINS const& margins) -> void {

        auto drawing_offset = margins.cyTopHeight;

        renderer->draw_image(tab_raster.get(), graphics::point<float> {229.f, drawing_offset - 28.0f}, 0.6f);
        renderer->draw_image(new_tab_symbol.get(), graphics::point<float> {460.f, drawing_offset - 22.0f}, 1.0f);

        renderer->draw_line(
            graphics::point<float> { window_rectangle.left, drawing_offset - 1.0f + 0.5f},
            graphics::point<float> { window_rectangle.right, drawing_offset - 1.0f + 0.5f},
            1.0f, graphics::color{ 0.77f, 0.77f, 0.77f, 1.0f }
        );

        renderer->draw_image(tab_raster.get(), graphics::point<float> {10.0f, drawing_offset - 28.0f}, 1.0f);

        renderer->draw_text(
            L"Expand the frame into t...",
            graphics::point<float>{ 48.f, drawing_offset - 24.f },
            500.0f, 50.0f, L"Segoe UI", 14.0f,
            graphics::color{ 0.4f, 0.4f, 0.4f, 1.0f }
        );

        renderer->draw_text(
            L"Recompute the window...",
            graphics::point<float>{ 221.f + 48.f, drawing_offset - 24.f },
            500.0f, 50.0f, L"Segoe UI", 14.0f,
            graphics::color{ 0.4f, 0.4f, 0.4f, 0.6f }
        );

    }

    auto application::paint_mock_caption() -> void {

        auto logo_to_draw = application_window_in_focus ? windows_logo.get() : windows_logo_dark.get();
        auto color = application_window_in_focus ? graphics::color{ 1.0f, 1.0f, 1.0f } : graphics::color{ 0.1f, 0.1f, 0.1f };

        renderer->draw_image(logo_to_draw, graphics::point<float> {12.f, 12.f}, 1.f);

        renderer->draw_text(
            window->get_title(),
            graphics::point<float>{ 48.f, 14.f },
            500.0f, 50.0f, L"Segoe UI", 14.0f,
            color, DWRITE_FONT_WEIGHT_SEMI_BOLD
        );
    }

    auto application::paint_mock_toolbar(graphics::rectangle<float> const& window_rectangle, MARGINS const& margins) -> void {

        renderer->draw_line(
            graphics::point<float> { window_rectangle.left, static_cast<float>(margins.cyTopHeight + 50) + .5f},
            graphics::point<float> { window_rectangle.right, static_cast<float>(margins.cyTopHeight + 50) + .5f},
            1.0f, graphics::color{ 0.82f, 0.82f, 0.82f, 1.0f }
        );

        renderer->draw_line(
            graphics::point<float> { window_rectangle.left, static_cast<float>(margins.cyTopHeight + 54) + .5f},
            graphics::point<float> { window_rectangle.right, static_cast<float>(margins.cyTopHeight + 54) + .5f},
            1.0f, graphics::color{ 0.82f, 0.82f, 0.82f, 1.0f }
        );

        renderer->fill_rectangle(
            graphics::rectangle<float> { 
                window_rectangle.left, static_cast<float>(margins.cyTopHeight + 55),
                window_rectangle.right, window_rectangle.bottom 
            }, graphics::color{ 1.0f, 1.0f, 1.0f }
        );

        renderer->fill_rectangle(
            graphics::rectangle<float> { 
                14.0f, static_cast<float>(margins.cyTopHeight + 9), window_rectangle.right - 14.0f, 
                static_cast<float>(margins.cyTopHeight + 41) 
            }, graphics::color{ 0.9f, 0.9f, 0.9f }
        );

        renderer->fill_rectangle(
            graphics::rectangle<float> { 
                15.0f, static_cast<float>(margins.cyTopHeight + 10), 
                window_rectangle.right - 15.0f, static_cast<float>(margins.cyTopHeight + 40) 
            }, graphics::color{ 1.0f, 1.0f, 1.0f }
        );

    }


    auto application::paint_mock_sidebar(graphics::rectangle<float> const& window_rectangle, MARGINS const& margins) -> void {

        renderer->fill_rectangle(
            graphics::rectangle<float> { 
                window_rectangle.left, static_cast<float>(margins.cyTopHeight + 55), 
                window_rectangle.left + 400.0f, window_rectangle.bottom 
            }, graphics::color{ 0.92f, 0.92f, 0.92f }
        );

    }
}