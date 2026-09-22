#include "launchpad/hal/linux_key_injector.hpp"
#include "launchpad/domain/key_definition.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <string>

namespace launchpad::hal {

using namespace launchpad::domain::key_codes;

LinuxKeyInjector::LinuxKeyInjector(std::string_view keypad_path, std::string_view fiveway_path) noexcept {
    keypad_fd_ = open(std::string(keypad_path).c_str(), O_WRONLY);
    fiveway_fd_ = open(std::string(fiveway_path).c_str(), O_WRONLY);
}

LinuxKeyInjector::~LinuxKeyInjector() noexcept {
    if (keypad_fd_ >= 0) close(keypad_fd_);
    if (fiveway_fd_ >= 0) close(fiveway_fd_);
}

bool LinuxKeyInjector::is_fiveway_code(core::RawKeyCode code) const noexcept {
    return code == KPKEY_FW_LEFT || code == KPKEY_FW_RIGHT ||
           code == KPKEY_FW_UP || code == KPKEY_FW_DOWN ||
           code == KPKEY_FW_PRESS;
}

void LinuxKeyInjector::write_to_fiveway(core::RawKeyCode code) {
    if (fiveway_fd_ < 0) return;
    char buffer[32];
    int length = snprintf(buffer, sizeof(buffer), "send %u\n", code);
    if (length > 0) {
        auto written = write(fiveway_fd_, buffer, static_cast<size_t>(length));
        (void)written;
    }
}

void LinuxKeyInjector::write_to_keypad(core::RawKeyCode code, domain::SendMode mode) {
    if (keypad_fd_ < 0) return;
    const char* prefix = "send";
    if (mode == domain::SendMode::Shift) prefix = "sendshift";
    if (mode == domain::SendMode::Alt) prefix = "sendalt";
    char buffer[32];
    int length = snprintf(buffer, sizeof(buffer), "%s %u\n", prefix, code);
    if (length > 0) {
        auto written = write(keypad_fd_, buffer, static_cast<size_t>(length));
        (void)written;
    }
}

core::Result<void> LinuxKeyInjector::inject_key(core::RawKeyCode code, domain::SendMode mode) {
    if (is_fiveway_code(code)) {
        write_to_fiveway(code);
        return core::Result<void>();
    }
    write_to_keypad(code, mode);
    return core::Result<void>();
}

core::Result<void> LinuxKeyInjector::inject_ascii_char(char c, const domain::AsciiTranslator& translator) {
    auto mapping = translator.translate(c);
    if (mapping.code != 0) {
        static_cast<void>(inject_key(mapping.code, mapping.send_mode));
        usleep(10000);
    }
    return core::Result<void>();
}

} // namespace launchpad::hal
