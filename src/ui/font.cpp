#include "launchpad/ui/font.hpp"
#include <cstddef>

namespace {
#include "fnt12x22.h"
constexpr uint32_t FONT_GLYPH_WIDTH = 12;
constexpr uint32_t FONT_GLYPH_HEIGHT = 22;
constexpr size_t BYTES_PER_ROW = 6;
constexpr size_t BYTES_PER_GLYPH = FONT_GLYPH_HEIGHT * BYTES_PER_ROW;
} // namespace

namespace launchpad::ui {

uint32_t FontRenderer::glyph_width() const noexcept {
    return FONT_GLYPH_WIDTH;
}

uint32_t FontRenderer::glyph_height() const noexcept {
    return FONT_GLYPH_HEIGHT;
}

void FontRenderer::render_glyph_row(hal::IDisplay& display, uint32_t x, uint32_t y, const uint8_t* row_data) const {
    for (size_t byte_idx = 0; byte_idx < BYTES_PER_ROW; ++byte_idx) {
        uint8_t byte_val = row_data[byte_idx];
        uint8_t left_nibble = (byte_val >> 4) & 0x0F;
        uint8_t right_nibble = byte_val & 0x0F;

        if (left_nibble > 0) {
            display.set_pixel(x + static_cast<uint32_t>(byte_idx * 2), y, 0x00);
        }
        if (right_nibble > 0) {
            display.set_pixel(x + static_cast<uint32_t>(byte_idx * 2 + 1), y, 0x00);
        }
    }
}

void FontRenderer::draw_char(hal::IDisplay& display, uint32_t x, uint32_t y, char c) const {
    auto uchar = static_cast<unsigned char>(c);
    const uint8_t* glyph_data = default_font + (uchar * BYTES_PER_GLYPH);

    for (uint32_t row = 0; row < FONT_GLYPH_HEIGHT; ++row) {
        render_glyph_row(display, x, y + row, glyph_data + (row * BYTES_PER_ROW));
    }
}

void FontRenderer::draw_string(hal::IDisplay& display, uint32_t x, uint32_t y, std::string_view text) const {
    uint32_t current_x = x;
    for (char c : text) {
        draw_char(display, current_x, y, c);
        current_x += FONT_GLYPH_WIDTH;
    }
}

} // namespace launchpad::ui
