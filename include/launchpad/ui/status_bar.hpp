/**
 * @file status_bar.hpp
 * @brief Bottom-right e-ink visual feedback overlay with background save and restore.
 * @details Implements domain::IStatusNotifier, providing transient visual feedback
 *          ("^[...]", "Success!", "Failure.") without destroying underlying screen pixels.
 */

#pragma once

#include "launchpad/domain/action_executor.hpp"
#include "launchpad/hal/display.hpp"
#include "launchpad/ui/font.hpp"
#include <vector>

namespace launchpad::ui {

/**
 * @brief Fixed height in pixels of the status bar overlay area.
 */
inline constexpr uint32_t STATUS_BAR_HEIGHT = 24;

/**
 * @brief Internal state and saved background pixel cache for the status bar.
 */
struct StatusBarState {
    const FontRenderer& font;           ///< Reference to font rasterizer.
    bool active{false};                 ///< Whether the status bar is currently visible on screen.
    std::vector<uint8_t> saved_pixels{};///< Backed up pixels copied before drawing overlay.
};

/**
 * @brief Visual status widget providing non-destructive e-ink on-screen feedback.
 */
class StatusBar : public domain::IStatusNotifier {
public:
    /**
     * @brief Constructs a StatusBar attached to the given display and font renderer.
     * @param[in,out] display Target IDisplay screen.
     * @param[in] font Font renderer for drawing feedback glyphs.
     */
    StatusBar(hal::IDisplay& display, const FontRenderer& font) noexcept;

    /**
     * @brief Displays an informational text message in the bottom-right corner.
     * @param[in] message Status text string to show.
     */
    void show_status(std::string_view message) override;

    /**
     * @brief Displays an action outcome indicator ("Success!" or "Failure.").
     * @param[in] success True if action succeeded, false if error.
     */
    void show_result(bool success) override;

    /**
     * @brief Restores saved background pixels and clears the status bar from screen.
     */
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
