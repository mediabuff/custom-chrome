#include <array>

#include <graphics/renderer.hpp>

namespace graphics {

    renderer::renderer() {

        std::uint32_t flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
        flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        std::array<D3D_FEATURE_LEVEL, 7> requested_levels{
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };

        D3D_FEATURE_LEVEL received_feature_level; // Any will do in this scenario.
        ID3D11Device* temporary_device; ID3D11DeviceContext* temporary_context;

        auto hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
            flags, requested_levels.data(), static_cast<std::uint32_t>(requested_levels.size()),
            D3D11_SDK_VERSION, &temporary_device, &received_feature_level, &temporary_context
        );

        device_context_d3d11 = com::make_unique(temporary_context);
        device_d3d11 = com::make_unique(temporary_device);

        ID2D1Factory* temporary_factory;
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &temporary_factory);
        factory_d2d1 = com::make_unique(temporary_factory);

        if (FAILED(hr)) throw std::runtime_error{ "Failed during the creation of the D2D1 device." };

        factory_d2d1->GetDesktopDpi(&dpiX, &dpiY);
        auto properties = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            dpiX, dpiY, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT
        );

        IDWriteFactory* temporary_factory_dwrite;
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_ISOLATED, __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&temporary_factory_dwrite)
        );
        factory_dwrite = com::make_unique(temporary_factory_dwrite);
        if (FAILED(hr)) throw std::runtime_error{ "Failed during creation of the DWrite factory." };

        hr = CoInitialize(nullptr);
        if (FAILED(hr)) throw std::runtime_error{ "Failed during CoInitialize." };

        IWICImagingFactory* temporary_factory_wic;
        hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&temporary_factory_wic));
        if (FAILED(hr)) throw std::runtime_error{ "Failed during the creation of the WIC imaging factory." };
        factory_wic = com::make_unique(temporary_factory_wic);

        IDWriteTextFormat* temporary_text_format;
        factory_dwrite->CreateTextFormat(
            L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"en_US", &temporary_text_format
        );
        standard_text_format = com::make_unique(temporary_text_format);

        IDXGIDevice* temporary_device_dxgi;
        device_d3d11->QueryInterface<IDXGIDevice>(&temporary_device_dxgi);

        ID2D1Device* temporary_device_d2d1;
        D2D1CreateDevice(
            temporary_device_dxgi,
            D2D1::CreationProperties(
                D2D1_THREADING_MODE_SINGLE_THREADED,
                D2D1_DEBUG_LEVEL_INFORMATION,
                D2D1_DEVICE_CONTEXT_OPTIONS_NONE
            ), &temporary_device_d2d1
        );
        device_d2d1 = com::make_unique(temporary_device_d2d1);

        ID2D1DeviceContext* temporary_device_context_d2d1;
        hr = device_d2d1->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &temporary_device_context_d2d1);
        resource_device_context_d2d1 = com::make_unique(temporary_device_context_d2d1);
        if (FAILED(hr)) throw std::runtime_error{ "Failed during the creation of the resource D2D device context." };

        IDCompositionDevice* temporary_device_dcomp;
        hr = DCompositionCreateDevice2(device_d2d1.get(), IID_PPV_ARGS(&temporary_device_dcomp));
        device_dcomp = com::make_unique(temporary_device_dcomp);
        if (FAILED(hr)) throw std::runtime_error{ "Failed during the creation of the DComp device." };

        temporary_device_dxgi->Release();

    }

    auto renderer::attach_to_window(HWND window_handle) -> void {

        associated_window = window_handle;

        IDCompositionTarget* temporary_target;
        auto hr = device_dcomp->CreateTargetForHwnd(window_handle, true, &temporary_target);
        window_target_dcomp = com::make_unique(temporary_target);
        if (FAILED(hr)) throw std::runtime_error{ "Failed during the creation of DComp window target." };

        resize_buffer();

    }

    auto renderer::resize_buffer() -> void {

        RECT window_rectangle;
        GetClientRect(associated_window, &window_rectangle);

        IDCompositionSurface* temporary_surface;
        device_dcomp->CreateSurface(
            window_rectangle.right, window_rectangle.bottom, 
            DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ALPHA_MODE_PREMULTIPLIED, 
            &temporary_surface
        );
        
        window_surface_dcomp = com::make_unique(temporary_surface);

    }

    auto renderer::begin_draw() -> void {

        IDCompositionVisual* temporary_visual;
        device_dcomp->CreateVisual(&temporary_visual);
        primary_visual_dcomp = com::make_unique(temporary_visual);

        ID2D1DeviceContext* temporary_device_context_d2d1; POINT offset = {};
        window_surface_dcomp->BeginDraw(nullptr, IID_PPV_ARGS(&temporary_device_context_d2d1), &offset);
        device_context_d2d1 = com::make_unique(temporary_device_context_d2d1);
        offsetX = static_cast<float>(offset.x);
        offsetY = static_cast<float>(offset.y);

        device_context_d2d1->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0, 0.0f));

        primary_visual_dcomp->SetContent(window_surface_dcomp.get());
        window_target_dcomp->SetRoot(primary_visual_dcomp.get());

        ID2D1SolidColorBrush* temporary_brush;
        device_context_d2d1->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f), &temporary_brush);
        brush = com::make_unique(temporary_brush);
 

    }

    auto renderer::end_draw() -> void {

        window_surface_dcomp->EndDraw();
        device_dcomp->Commit();
        primary_visual_dcomp.reset(nullptr);

    }

    auto renderer::fill_rectangle(rectangle<float> const& fill_area, color const& fill_color) -> void {

        brush->SetColor(D2D1::ColorF(fill_color.r, fill_color.g, fill_color.b, fill_color.a));

        device_context_d2d1->FillRectangle(
            D2D1::RectF(
                fill_area.left + offsetX, fill_area.top + offsetY,
                fill_area.right + offsetX, fill_area.bottom + offsetY
            ), brush.get()
        );

    }

    auto renderer::draw_line(point<float> const& start_point, point<float> const& end_point, float const stroke_width, color const& stroke_color) -> void {

        brush->SetColor(D2D1::ColorF(stroke_color.r, stroke_color.g, stroke_color.b, stroke_color.a));

        device_context_d2d1->DrawLine(
            D2D1::Point2F(start_point.x + offsetX, start_point.y + offsetY),
            D2D1::Point2F(end_point.x + offsetX, end_point.y + offsetY),
            brush.get(), stroke_width
        );

    }

    auto renderer::draw_image(resource::image const * image, point<float> const & top_left, float const opacity) -> void {

        auto resident_texture = get_texture(image->get_file_path());
        auto dimensions = resident_texture->GetPixelSize();

        device_context_d2d1->DrawBitmap(
            resident_texture,
            D2D1::RectF(
                top_left.x + offsetX, top_left.y + offsetY,
                top_left.x + static_cast<float>(dimensions.width) + offsetX,
                top_left.y + static_cast<float>(dimensions.height) + offsetY
            ),
            opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
        );

    }

    auto renderer::draw_text(
        std::wstring const& text, point<float> const& top_left,
        float const max_width, float const max_height, 
        std::wstring font_family, float const font_size, 
        color const& text_color, DWRITE_FONT_WEIGHT const font_weight
    ) -> void {

        // Don't really care about precomputing text layouts, we just regenerate them on each draw (not frequent here).
        IDWriteTextLayout* temporary_text_layout;

        factory_dwrite->CreateTextLayout(
            text.c_str(), static_cast<std::uint32_t>(text.size()),
            standard_text_format.get(), max_width, max_height, &temporary_text_layout
        );

        com::unique_ptr<IDWriteTextLayout> text_layout{ temporary_text_layout };

        DWRITE_TEXT_RANGE text_range{}; text_range.length = static_cast<std::uint32_t>(text.size());
        text_layout->SetFontFamilyName(font_family.c_str(), text_range);
        text_layout->SetFontWeight(font_weight, text_range);
        text_layout->SetFontSize(font_size, text_range);

        brush->SetColor(D2D1::ColorF(text_color.r, text_color.g, text_color.b, text_color.a));
        device_context_d2d1->DrawTextLayout(
            D2D1::Point2F(top_left.x + offsetX, top_left.y + offsetY),
            text_layout.get(), brush.get()
        );

    }

    auto renderer::get_texture(std::wstring const& filename) -> ID2D1Bitmap* {
        auto lookup_iterator = resident_textures_map.find(filename);
        return lookup_iterator != resident_textures_map.end() ? lookup_iterator->second.get() : load_image_into_pool(filename);
    }

    auto renderer::load_image_into_pool(std::wstring const& filename) -> ID2D1Bitmap* {

        IWICBitmapDecoder* temporary_bitmap_decoder;
        factory_wic->CreateDecoderFromFilename(
            filename.c_str(), nullptr, GENERIC_READ, 
            WICDecodeOptions::WICDecodeMetadataCacheOnLoad, 
            &temporary_bitmap_decoder
        );
        com::unique_ptr<IWICBitmapDecoder> bitmap_decoder{ temporary_bitmap_decoder };

        IWICBitmapFrameDecode* temporary_frame_decode;
        bitmap_decoder->GetFrame(0, &temporary_frame_decode);
        com::unique_ptr<IWICBitmapFrameDecode> zero_frame{ temporary_frame_decode };

        IWICFormatConverter* temporary_format_converter;
        factory_wic->CreateFormatConverter(&temporary_format_converter);
        com::unique_ptr<IWICFormatConverter> format_converter{ temporary_format_converter };

        format_converter->Initialize(
            zero_frame.get(), GUID_WICPixelFormat32bppPBGRA, 
            WICBitmapDitherTypeNone, nullptr, 0.0f, 
            WICBitmapPaletteTypeMedianCut
        );

        ID2D1Bitmap* temporary_bitmap;
        resource_device_context_d2d1->CreateBitmapFromWicBitmap(format_converter.get(), &temporary_bitmap);
        resident_textures_map.emplace(filename, temporary_bitmap);

        return temporary_bitmap;

    }

}