// Copyright ©2016 Domagoj "oberth" Pandža
// MIT license | Read LICENSE.txt for details.

#pragma once

#include <cstdint>

namespace graphics {
    template<typename T>
    struct rectangle {
        T left, top, right, bottom;

        rectangle() = default;
        rectangle(T const left, T const top, T const right, T const bottom)
        : left(left), top(top), right(right), bottom(bottom) {}

    };

    template<typename T>
    struct point {
        T x, y;

        point() = default;
        point(T const x, T const y)
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