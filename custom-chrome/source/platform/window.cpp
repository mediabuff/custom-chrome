#include <platform/window.hpp>

namespace platform {

    window::window(std::wstring const & title, std::uint32_t const width, std::uint32_t const height, std::uint32_t const caption_height, void* application_ptr, window_event_handler event_handler) : title(title) {

        WNDCLASSEX window_class{};

        margins.cxLeftWidth = margins.cxRightWidth = margins.cyBottomHeight = 0;
        margins.cyTopHeight = caption_height;

        window_class.cbSize = sizeof window_class;
        window_class.cbWndExtra = sizeof application_ptr;
        window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        window_class.hInstance = GetModuleHandleW(nullptr);
        window_class.lpfnWndProc = event_handler;
        window_class.lpszClassName = L"window_class";
        window_class.style = CS_VREDRAW | CS_HREDRAW;
        // Do the icon for the taskbar yourself. :P

        RegisterClassEx(&window_class);

        RECT window_rectangle{};
        window_rectangle.top = 100;
        window_rectangle.left = 100;
        window_rectangle.right = width + window_rectangle.left;
        window_rectangle.bottom = height + window_rectangle.top;
        AdjustWindowRect(&window_rectangle, WS_OVERLAPPEDWINDOW, false);

        auto x = window_rectangle.left;
        auto y = window_rectangle.top;
        auto adjusted_width = window_rectangle.right - window_rectangle.left;
        auto adjusted_height = window_rectangle.bottom - window_rectangle.top;

        system_window_handle = CreateWindowExW(
            WS_EX_NOREDIRECTIONBITMAP, window_class.lpszClassName, title.c_str(), WS_OVERLAPPEDWINDOW, x, y,
            adjusted_width, adjusted_height, nullptr, nullptr, window_class.hInstance, application_ptr
        );

        if (system_window_handle == nullptr) throw std::runtime_error{ "Failed to create a window." };

        auto hr = DwmExtendFrameIntoClientArea(system_window_handle, &margins);
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
