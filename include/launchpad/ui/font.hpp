/**
 * @file font.hpp
 * @brief Embedded 12x22 bitmap font renderer for Kindle Launchpad.
 * @details Renders ASCII characters and text strings into any IDisplay framebuffer
 *          using the embedded 1-bit bitmap font tables.
 */

#pragma once

#include "launchpad/hal/display.hpp"
#include <cstdint>
#include <string_view>

namespace launchpad::ui {

/**
 * @brief Rasterizer for the embedded 12x22 pixel monochrome bitmap font.
 */
class FontRenderer {
public:
    FontRenderer() noexcept = default;

    /**
     * @brief Returns the fixed width of each glyph in pixels (12 px).
     * @return Glyph width in pixels.
     */
    [[nodiscard]] uint32_t glyph_width() const noexcept;

    /**
     * @brief Returns the fixed height of each glyph in pixels (22 px).
     * @return Glyph height in pixels.
     */
    [[nodiscard]] uint32_t glyph_height() const noexcept;

    /**
     * @brief Draws a single ASCII character glyph at the specified display coordinates.
     * @param[in,out] display Target IDisplay framebuffer.
     * @param[in] x Top-left horizontal coordinate.
     * @param[in] y Top-left vertical coordinate.
     * @param[in] c ASCII character to rasterize.
     */
    void draw_char(hal::IDisplay& display, uint32_t x, uint32_t y, char c) const;

    /**
     * @brief Draws a string of ASCII characters horizontally starting at (x, y).
     * @param[in,out] display Target IDisplay framebuffer.
     * @param[in] x Starting horizontal pixel coordinate.
     * @param[in] y Starting vertical pixel coordinate.
     * @param[in] text Text string to render.
     */
    void draw_string(hal::IDisplay& display, uint32_t x, uint32_t y, std::string_view text) const;

private:
    void render_glyph_row(hal::IDisplay& display, uint32_t x, uint32_t y, const uint8_t* row_data) const;
};

} // namespace launchpad::ui
