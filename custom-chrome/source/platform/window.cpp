#include <platform/window.hpp>
#include <cmath>

namespace platform {

    window::window(std::wstring const & title, std::uint32_t const width, std::uint32_t const height, std::uint32_t const caption_height, void* application_ptr, window_event_handler event_handler) : title(title) {

        WNDCLASSEX window_class{};

        auto dpi = GetDpiForSystem();
        user_scaling = static_cast<float>(dpi) / 96.0f;

        MARGINS physical_margins { 0, 0, static_cast<int>(std::ceilf(user_scaling * caption_height)), 0 };
        margins = { 0, 0, static_cast<int>(caption_height), 0 };

        window_class.cbSize = sizeof window_class;
        window_class.cbWndExtra = sizeof application_ptr;
        window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        window_class.hInstance = GetModuleHandleW(nullptr);
        window_class.lpfnWndProc = event_handler;
        window_class.lpszClassName = L"window_class";
        window_class.style = CS_VREDRAW | CS_HREDRAW;

        RegisterClassEx(&window_class);

        RECT window_rectangle { 0, 0, static_cast<int>(width), static_cast<int>(height)};
        AdjustWindowRect(&window_rectangle, WS_OVERLAPPEDWINDOW, false);

        auto x = static_cast<int>(10 * user_scaling), y = x;
        auto adjusted_width = window_rectangle.right - window_rectangle.left;
        auto adjusted_height = window_rectangle.bottom - window_rectangle.top;
        adjusted_width = static_cast<int>(user_scaling * adjusted_width);
        adjusted_height = static_cast<int>(user_scaling * adjusted_height);

        system_window_handle = CreateWindowExW(
            WS_EX_NOREDIRECTIONBITMAP, window_class.lpszClassName, title.c_str(), WS_OVERLAPPEDWINDOW, x, y,
            adjusted_width, adjusted_height, nullptr, nullptr, window_class.hInstance, application_ptr
        );

        if (system_window_handle == nullptr) throw std::runtime_error{ "Failed to create a window." };

        auto hr = DwmExtendFrameIntoClientArea(system_window_handle, &physical_margins);
        if (FAILED(hr)) throw std::runtime_error { "DWM failed to extend frame into the client area." };

    }

    window::~window() {
        DestroyWindow(system_window_handle);
        UnregisterClassW(L"window_class", GetModuleHandle(nullptr));
    }

    auto window::show_window() -> void {

        ShowWindow(system_window_handle, SW_SHOW);
        UpdateWindow(system_window_handle);

    }
}
