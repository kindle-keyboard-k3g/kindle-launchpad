#pragma once

#include "launchpad/hal/display.hpp"
#include <cstdint>
#include <string_view>

namespace launchpad::ui {

class FontRenderer {
public:
    FontRenderer() noexcept = default;

    [[nodiscard]] uint32_t glyph_width() const noexcept;
    [[nodiscard]] uint32_t glyph_height() const noexcept;

    void draw_char(hal::IDisplay& display, uint32_t x, uint32_t y, char c) const;
    void draw_string(hal::IDisplay& display, uint32_t x, uint32_t y, std::string_view text) const;

private:
    void render_glyph_row(hal::IDisplay& display, uint32_t x, uint32_t y, const uint8_t* row_data) const;
};

} // namespace launchpad::ui
