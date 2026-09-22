/**
 * @file screenshot_writer.hpp
 * @brief Exports Kindle e-ink framebuffer memory to uncompressed BMP image files.
 * @details Reads 4bpp/8bpp pixels from an IDisplay and serializes a standard Windows BMP
 *          image with BITMAPFILEHEADER, BITMAPINFOHEADER, grayscale color palette, and padded rows.
 */

#pragma once

#include "launchpad/core/result.hpp"
#include "launchpad/hal/display.hpp"
#include <string_view>
#include <fstream>

namespace launchpad::ui {

/**
 * @brief Static utility writing display framebuffer contents to BMP image files.
 */
class ScreenshotWriter {
public:
    /**
     * @brief Captures the current display pixels and saves them as an uncompressed 8bpp BMP file.
     * @param[in] display Source IDisplay framebuffer.
     * @param[in] file_path Target file destination path (e.g. /mnt/us/screenshot.bmp).
     * @return Result<void> indicating success or filesystem write error.
     */
    static core::Result<void> write_bmp(const hal::IDisplay& display, std::string_view file_path);

private:
    static void write_headers(std::ofstream& file, uint32_t width, uint32_t height, uint32_t stride);
    static void write_palette(std::ofstream& file);
    static void write_pixel_rows(std::ofstream& file, const hal::IDisplay& display, uint32_t width, uint32_t height, uint32_t stride);
};

} // namespace launchpad::ui
