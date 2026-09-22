#include "launchpad/ui/status_bar.hpp"

namespace launchpad::ui {

StatusBar::StatusBar(hal::IDisplay& display, const FontRenderer& font) noexcept
    : display_(display),
      state_{font} {}

hal::UpdateRegion StatusBar::get_bar_region() const noexcept {
    auto bounds = display_.bounds();
    uint32_t y = (bounds.height > STATUS_BAR_HEIGHT) ? (bounds.height - STATUS_BAR_HEIGHT) : 0;
    return hal::UpdateRegion{0, y, bounds.width, STATUS_BAR_HEIGHT};
}

void StatusBar::capture_background() {
    auto region = get_bar_region();
    state_.saved_pixels.clear();
    state_.saved_pixels.reserve(region.width * region.height);
    for (uint32_t y = region.y; y < region.y + region.height; ++y) {
        for (uint32_t x = 0; x < region.width; ++x) {
            state_.saved_pixels.push_back(display_.get_pixel(x, y));
        }
    }
    state_.active = true;
}

void StatusBar::restore_background() {
    if (!state_.active || state_.saved_pixels.empty()) {
        return;
    }
    auto region = get_bar_region();
    size_t idx = 0;
    for (uint32_t y = region.y; y < region.y + region.height; ++y) {
        for (uint32_t x = 0; x < region.width; ++x) {
            display_.set_pixel(x, y, state_.saved_pixels[idx++]);
        }
    }
    state_.saved_pixels.clear();
    state_.active = false;
    static_cast<void>(display_.update(region));
}

void StatusBar::draw_bar_text(std::string_view text) {
    auto region = get_bar_region();
    for (uint32_t y = region.y; y < region.y + region.height; ++y) {
        for (uint32_t x = 0; x < region.width; ++x) {
            display_.set_pixel(x, y, 0xFF);
        }
    }
    state_.font.draw_string(display_, 30, region.y + 1, text);
    static_cast<void>(display_.update(region));
}

void StatusBar::show_status(std::string_view message) {
    if (!state_.active) {
        capture_background();
    }
    draw_bar_text(message);
}

void StatusBar::show_result(bool success) {
    if (success) {
        draw_bar_text("Success!");
        return;
    }
    draw_bar_text("Failure.");
}

void StatusBar::hide_status() {
    restore_background();
}

} // namespace launchpad::ui
