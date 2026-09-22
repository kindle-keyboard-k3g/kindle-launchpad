#pragma once

#include "launchpad/hal/key_injector.hpp"
#include <string_view>

namespace launchpad::hal {

class LinuxKeyInjector : public IKeyInjector {
public:
    explicit LinuxKeyInjector(
        std::string_view keypad_path = "/proc/keypad",
        std::string_view fiveway_path = "/proc/fiveway"
    ) noexcept;
    ~LinuxKeyInjector() noexcept;

    LinuxKeyInjector(const LinuxKeyInjector&) = delete;
    LinuxKeyInjector& operator=(const LinuxKeyInjector&) = delete;

    core::Result<void> inject_key(core::RawKeyCode code, domain::SendMode mode) override;
    core::Result<void> inject_ascii_char(char c, const domain::AsciiTranslator& translator) override;

private:
    int keypad_fd_{-1};
    int fiveway_fd_{-1};

    void write_to_keypad(core::RawKeyCode code, domain::SendMode mode);
    void write_to_fiveway(core::RawKeyCode code);
    [[nodiscard]] bool is_fiveway_code(core::RawKeyCode code) const noexcept;
};

} // namespace launchpad::hal
