#pragma once

#include "launchpad/domain/action_executor.hpp"
#include "launchpad/hal/display.hpp"
#include "launchpad/ui/font.hpp"
#include <vector>

namespace launchpad::ui {

inline constexpr uint32_t STATUS_BAR_HEIGHT = 24;

struct StatusBarState {
    const FontRenderer& font;
    bool active{false};
    std::vector<uint8_t> saved_pixels{};
};

class StatusBar : public domain::IStatusNotifier {
public:
    StatusBar(hal::IDisplay& display, const FontRenderer& font) noexcept;

    void show_status(std::string_view message) override;
    void show_result(bool success) override;
    void hide_status() override;

private:
    void capture_background();
    void restore_background();
    void draw_bar_text(std::string_view text);
    [[nodiscard]] hal::UpdateRegion get_bar_region() const noexcept;

    hal::IDisplay& display_;
    StatusBarState state_;
};

} // namespace launchpad::ui
