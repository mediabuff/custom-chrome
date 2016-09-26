// Copyright ©2016 Domagoj "oberth" Pandža
// MIT license | Read LICENSE.txt for details.

#pragma once

#include <memory>
#include <cstdint>
#include <stdexcept>

namespace graphics {
    template<typename t>
    struct rectangle {
        t left, top, right, bottom;

        rectangle() = default;
        rectangle(t const left, t const top, t const right, t const bottom)
        : left(left), top(top), right(right), bottom(bottom) {}

    };

    template<typename t>
    struct point {
        t x, y;

        point() = default;
        point(t const x, t const y)
        : x(x), y(y) {}

    };

    struct color {
        float r, g, b, a;

        color(std::uint8_t const r, std::uint8_t const g, std::uint8_t const b, std::uint8_t const a = 255)
            : r(static_cast<float>(r) / 255.0f), g(static_cast<float>(g) / 255.0f),
            b(static_cast<float>(b) / 255.0f), a(static_cast<float>(a) / 255.0f) {}

        color(float const r, float const g, float const b, float const a = 1.0f)
        : r(r), g(g), b(b), a(a) {}

    };
}

template <typename com_type>
struct com_deleter {
    void operator()(com_type* ptr) {
        ptr->Release();
    }
};

template <typename com_type>
using unique_com_ptr = std::unique_ptr<com_type, com_deleter<com_type>>;