/**
 * @file linux_key_injector.hpp
 * @brief Concrete Linux key injector writing to /proc/keypad and /proc/fiveway.
 * @details Synthesizes virtual key strokes and 5-way joystick movements by writing
 *          ASCII key definitions or scancodes into the Amazon kernel driver interfaces.
 */

#pragma once

#include "launchpad/hal/key_injector.hpp"
#include <string_view>

namespace launchpad::hal {

/**
 * @brief Concrete IKeyInjector communicating with the Kindle Linux kernel /proc drivers.
 */
class LinuxKeyInjector : public IKeyInjector {
public:
    /**
     * @brief Constructs a LinuxKeyInjector opening /proc/keypad and /proc/fiveway.
     * @param[in] keypad_path Filesystem path to the keypad injection node (defaults to /proc/keypad).
     * @param[in] fiveway_path Filesystem path to the 5-way injection node (defaults to /proc/fiveway).
     */
    explicit LinuxKeyInjector(
        std::string_view keypad_path = "/proc/keypad",
        std::string_view fiveway_path = "/proc/fiveway"
    ) noexcept;

    /**
     * @brief Destructor closing open /proc file descriptors.
     */
    ~LinuxKeyInjector() noexcept;

    LinuxKeyInjector(const LinuxKeyInjector&) = delete;
    LinuxKeyInjector& operator=(const LinuxKeyInjector&) = delete;

    /**
     * @brief Injects a physical key scancode into the kernel driver with optional modifiers.
     * @param[in] code Raw evdev scancode.
     * @param[in] mode Modifier combination (Normal, Shift, Alt).
     * @return Result<void> indicating whether write to /proc node succeeded.
     */
    core::Result<void> inject_key(core::RawKeyCode code, domain::SendMode mode) override;

    /**
     * @brief Injects an ASCII character by translating it and writing scancodes or symbol steps.
     * @param[in] c ASCII character to emit.
     * @param[in] translator AsciiTranslator configured for target Kindle model.
     * @return Result<void> indicating success or injection failure.
     */
    core::Result<void> inject_ascii_char(char c, const domain::AsciiTranslator& translator) override;

private:
    int keypad_fd_{-1};
    int fiveway_fd_{-1};

    void write_to_keypad(core::RawKeyCode code, domain::SendMode mode);
    void write_to_fiveway(core::RawKeyCode code);
    [[nodiscard]] bool is_fiveway_code(core::RawKeyCode code) const noexcept;
};

} // namespace launchpad::hal
