#pragma once

#include "launchpad/core/result.hpp"
#include "launchpad/hal/display.hpp"
#include <string_view>

namespace launchpad::ui {

class ScreenshotWriter {
public:
    static core::Result<void> write_bmp(const hal::IDisplay& display, std::string_view file_path);

private:
    static void write_headers(std::ofstream& file, uint32_t width, uint32_t height, uint32_t stride);
    static void write_palette(std::ofstream& file);
    static void write_pixel_rows(std::ofstream& file, const hal::IDisplay& display, uint32_t width, uint32_t height, uint32_t stride);
};

} // namespace launchpad::ui
