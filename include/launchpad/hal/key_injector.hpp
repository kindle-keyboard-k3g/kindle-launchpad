/**
 * @file key_injector.hpp
 * @brief Abstract interface for virtual keystroke synthesis and injection.
 * @details Declares IKeyInjector, enabling simulated keyboard typing into active
 *          applications without relying on physical hardware key actuation.
 */

#pragma once

#include "launchpad/core/result.hpp"
#include "launchpad/core/units.hpp"
#include "launchpad/domain/ascii_translator.hpp"

namespace launchpad::hal {

/**
 * @brief Abstract interface for synthesizing virtual hardware keystrokes and ASCII text entry.
 */
class IKeyInjector {
public:
    virtual ~IKeyInjector() = default;

    /**
     * @brief Injects a single hardware key scancode with specified modifier state.
     * @param[in] code Raw evdev key code to simulate.
     * @param[in] mode Modifier state (Normal, Shift, or Alt).
     * @return Result<void> indicating success or injection failure.
     */
    virtual core::Result<void> inject_key(core::RawKeyCode code, domain::SendMode mode) = 0;

    /**
     * @brief Injects an ASCII character using the provided AsciiTranslator model.
     * @param[in] c ASCII character to emit.
     * @param[in] translator Model-specific ASCII key translator.
     * @return Result<void> indicating success or injection failure.
     */
    virtual core::Result<void> inject_ascii_char(char c, const domain::AsciiTranslator& translator) = 0;
};

} // namespace launchpad::hal
