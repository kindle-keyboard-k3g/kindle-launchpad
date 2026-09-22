/**
 * @file ascii_translator.hpp
 * @brief Translates ASCII characters into Kindle framework keystroke actions.
 * @details Converts alphanumeric characters, punctuation, and symbols into keyboard scancodes,
 *          modifier combinations (Shift, Alt), or symbol menu 5-way joystick navigation steps.
 */

#pragma once

#include "launchpad/core/units.hpp"
#include <cstdint>
#include <optional>

namespace launchpad::domain {

/**
 * @brief Keystroke modifier mode required to type a specific character.
 */
enum class SendMode : uint8_t {
    Normal = 0, ///< Direct keystroke without modifiers.
    Shift = 1,  ///< Keystroke requiring the Shift modifier.
    Alt = 2     ///< Keystroke requiring the Alt modifier (used on Kindle DX/K2).
};

/**
 * @brief Target Kindle device hardware family.
 */
enum class KindleModel : uint8_t {
    KindleDX = 0, ///< Kindle DX, Kindle DX Graphite, Kindle 2.
    Kindle3 = 1   ///< Kindle Keyboard (Kindle 3).
};

/**
 * @brief 5-way joystick navigation offsets for characters selected via the symbol menu.
 */
struct SymbolNavigation {
    int8_t horizontal_steps{0}; ///< Left/right 5-way joystick steps (negative = left, positive = right).
    int8_t vertical_steps{0};   ///< Up/down 5-way joystick steps (negative = up, positive = down).
};

/**
 * @brief Complete mapping definition for emitting a single ASCII character.
 */
struct AsciiKeyMapping {
    core::RawKeyCode code{0};                          ///< Physical key code to inject.
    SendMode send_mode{SendMode::Normal};              ///< Modifier combination required.
    std::optional<SymbolNavigation> symbol_navigation{std::nullopt}; ///< Optional symbol menu navigation sequence.
};

/**
 * @brief Domain service translating ASCII characters into device-specific keystroke sequences.
 */
class AsciiTranslator {
public:
    /**
     * @brief Constructs an AsciiTranslator calibrated for the specified Kindle model.
     * @param[in] model Target Kindle hardware model family (defaults to KindleDX).
     */
    explicit AsciiTranslator(KindleModel model = KindleModel::KindleDX) noexcept;

    /**
     * @brief Translates an ASCII character into a physical key mapping.
     * @param[in] c ASCII character to translate.
     * @return AsciiKeyMapping containing keycode, modifiers, and symbol menu steps.
     */
    [[nodiscard]] AsciiKeyMapping translate(char c) const noexcept;

    /**
     * @brief Returns the active Kindle model configuration.
     * @return KindleModel enum value.
     */
    [[nodiscard]] KindleModel model() const noexcept;

    /**
     * @brief Updates the target Kindle model calibration.
     * @param[in] model New KindleModel to calibrate for.
     */
    void set_model(KindleModel model) noexcept;

private:
    KindleModel model_;
};

} // namespace launchpad::domain
