// Copyright ©2016 Domagoj "oberth" Pandža
// MIT license | Read LICENSE.txt for details.

#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dcomp.h>
#include <wincodec.h>
#include <unordered_map>
#include <string>

#include "common.hpp"
#include "image.hpp"

namespace graphics {

    // Just a simple renderer.
    class renderer {

    public:

        renderer();

        auto attach_to_window(HWND window_handle) -> void;
        auto resize_buffer() -> void;
        auto begin_draw() -> void;
        auto end_draw() -> void;

        auto fill_rectangle(rectangle<float> const& fill_area, color const& fill_color) -> void;
        auto draw_line(point<float> const& start, point<float> const& end, float const stroke_width, color const& stroke_color) -> void;
        auto draw_image(resource::image const* image, point<float> const& top_left, float const opacity) -> void;

        auto draw_text(
            std::wstring const& text, point<float> const& top_left,
            float const max_width, float const max_height,
            std::wstring font_family, float const font_size,
            color const& text_color, DWRITE_FONT_WEIGHT const font_weight = DWRITE_FONT_WEIGHT_NORMAL
        ) -> void;

    private:

        auto get_texture(std::wstring const& filename)-> ID2D1Bitmap*;
        auto load_image_into_pool(std::wstring const& filename) -> ID2D1Bitmap*;

        unique_com_ptr<ID3D11Device> device_d3d11;
        unique_com_ptr<ID3D11DeviceContext> device_context_d3d11;
        unique_com_ptr<ID2D1Factory> factory_d2d1;
        unique_com_ptr<ID2D1RenderTarget> dxgi_render_target_d2d1;
        unique_com_ptr<IDWriteFactory> factory_dwrite;
        unique_com_ptr<IWICImagingFactory> factory_wic;

        unique_com_ptr<ID2D1Device> device_d2d1;
        unique_com_ptr<ID2D1DeviceContext> device_context_d2d1;
        unique_com_ptr<ID2D1DeviceContext> resource_device_context_d2d1;

        unique_com_ptr<ID2D1SolidColorBrush> brush;
        unique_com_ptr<IDWriteTextFormat> standard_text_format;

        unique_com_ptr<IDCompositionDevice> device_dcomp;
        unique_com_ptr<IDCompositionTarget> window_target_dcomp;
        unique_com_ptr<IDCompositionVisual> primary_visual_dcomp;
        unique_com_ptr<IDCompositionSurface> window_surface_dcomp;
        unique_com_ptr<IDXGISurface> window_surface_dxgi;

        std::unordered_map<std::wstring, unique_com_ptr<ID2D1Bitmap>> resident_textures_map;

        HWND associated_window;

        float dpiX, dpiY;
        float offsetX, offsetY;

    };

}