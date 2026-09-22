#pragma once

#include <string>
#include <string_view>

namespace launchpad::system {

struct DevicePaths {
    std::string keyboard;
    std::string fiveway;
};

class CliOptions {
public:
    [[nodiscard]] bool is_foreground() const noexcept {
        return foreground_;
    }

    [[nodiscard]] const std::string& keyboard_device() const noexcept {
        return devices_.keyboard;
    }

    [[nodiscard]] const std::string& fiveway_device() const noexcept {
        return devices_.fiveway;
    }

    static CliOptions parse(int argc, char* const argv[]);

private:
    DevicePaths devices_{};
    bool foreground_{false};

    static void parse_flag(CliOptions& options, std::string_view argument);
};

} // namespace launchpad::system
