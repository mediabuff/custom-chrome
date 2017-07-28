// Copyright ©2016 Domagoj "oberth" Pandža
// MIT license | Read LICENSE.txt for details.

#pragma once
#include <string>

// Extremely simple image resource, does nothing but formulate a handle essentially.
// In the mockup application scenario, we just make every resource resident.
// Again, this is just serves a visualization bonus on top of the explanation how to draw custom frames.
namespace resource {
    struct image {

        image() = delete;
        image(std::wstring const& file_path) : file_path(file_path) {}

        image(image const&) = default;
        image(image&&) = default;
        image& operator=(image const&) = default;
        image& operator=(image&&) = default;

        ~image() = default;

        auto& get_file_path() const { return file_path; }
    private:
        std::wstring file_path;
    };
}
